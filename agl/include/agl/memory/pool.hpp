#pragma once
#include <cstddef>
#include "agl/core/application.hpp"
#include "agl/core/logger.hpp"
#include "agl/set.hpp"
#include "agl/dictionary.hpp"
#include "agl/memory/allocator.hpp"

namespace agl
{
namespace mem
{
namespace impl 
{
class free_space_tracker
{
public:
	bool has_space() const noexcept
	{
		return !m_spaces.empty();
	}
	// Adds item to the collection. If the space, that the object represents, overlaps with space of another object, the objects will be joined together.
	void push(std::byte* ptr, std::uint64_t size) noexcept
	{
		// find and merge adjacent spaces
		size += find_erase_adjacent_right(ptr + size, size);
		size += find_erase_adjacent_left(ptr);

		// emplace new space
		auto it_spaces = m_spaces.find(size);
		if (it_spaces == m_spaces.end())
			it_spaces = m_spaces.emplace({ size, set<std::byte*>{} });
		it_spaces->second.insert(ptr);
	}
	// Pops item of 'minimal_size' from the collection. If there are only items having more space than requested, 
	// the object with requested amount of space will be returned and the remaining space pushed back to the collection.
	std::byte* pop(std::uint64_t size) noexcept
	{
		auto it = std::lower_bound(m_spaces.begin(), m_spaces.end(), size, m_spaces.key_value_comp());
			
		AGL_ASSERT(it != m_spaces.end(), "No more memory available");

		auto* result = it->second.back();
		it->second.erase(it->second.cend() - 1);

		auto surplus = it->first - size; 
		if (it->second.empty())
			m_spaces.erase(it);

		if (surplus > 0) // check if there is excessive space available under pointer 'result'
		{
			auto* ptr = result + size;
			auto found = m_spaces.find(surplus); // where to insert new ptr
			if (found != m_spaces.end())
				found->second.insert(ptr);
			else
			{
				auto s_it = m_spaces.emplace({ surplus, set<std::byte*>{} });
				s_it->second.insert(ptr);
			}
		}

		return result;
	}

private:
	using spaces_type = dictionary<std::uint64_t, set<std::byte*>>; // size, pointers to blocks of 'size'

private:
	std::uint64_t find_erase_adjacent_right(std::byte* ptr, std::uint64_t size) noexcept
	{
		auto original_size = size;
		auto it_spaces = m_spaces.begin();
		for (it_spaces; it_spaces != m_spaces.end(); ++it_spaces)
		{
			// check for end_ptr in set
			auto it_second = it_spaces->second.find(ptr);
			if (it_second == it_spaces->second.end())
				continue;

			// remove pointer from set
			it_spaces->second.erase(it_second);

			// calc new end pointer and add size to 'to be emplaced' space
			ptr += it_spaces->first;
			size += it_spaces->first;

			// erase size gap if it was the last pointer to it
			if (it_spaces->second.empty())
				m_spaces.erase(it_spaces);

			// find new end_ptr
			it_spaces = m_spaces.begin();
		}

		return size - original_size;
	}

	std::uint64_t find_erase_adjacent_left(std::byte* ptr) noexcept
	{
		auto result = std::uint64_t{};
		for (auto it = m_spaces.begin(); it != m_spaces.end(); ++it)
		{
			auto it_ptrs = it->second.find(ptr - it->first);
			if (it_ptrs == it->second.end())
				continue;

			it->second.erase(it_ptrs);

			ptr -= it->first;
			result += it->first;

			if (it->second.empty())
				m_spaces.erase(it);

			if (m_spaces.empty())
				break;
			else
				it = m_spaces.begin();
		}

		return result;
	}

private:
	spaces_type m_spaces;
};
}

/**
 * @brief 
 * Provides a pool of memory, which can serve as a fast storage. Is not thread-safe.
 * Is a resource.
 * 
 * @dependecies
 * 'application'
 * 'logger'
 */
class pool
	: public resource<pool>
{
public:
	template <typename T>
	class allocator
	{
	public:
		static_assert(!std::is_const_v<T>, "allocator<const T> is ill-formed");
		static_assert(!std::is_function_v<T>, "[allocator.requirements]");
		static_assert(!std::is_reference_v<T>, "[allocator.requirements]");

	public:
		template <typename U>
		using rebind = allocator<U>;
		using value_type = T;
		using pointer = T*;
		using const_pointer = T const*;
		using reference = T&;
		using const_reference = T const&;
		using size_type = std::uint64_t;
		using difference_type = std::ptrdiff_t;

		allocator(pool* ptr = nullptr) noexcept
			: m_pool{ ptr }
		{
			m_alloc_count = 0;
		}
		allocator(allocator&& other) noexcept
			: m_pool{ other.m_pool }
		{
			m_alloc_count = other.m_alloc_count;
			other.m_alloc_count = 0;
		}
		template <typename U>
		allocator(allocator<U>&& other) noexcept
			: m_pool{ other.m_pool }
		{
			m_alloc_count = other.m_alloc_count;
			other.m_alloc_count = 0;
		}
		allocator(allocator const& other) noexcept
			: m_pool{ other.m_pool }
		{
			m_alloc_count = 0;
		}
		template <typename U>
		allocator(allocator<U> const& other) noexcept
			: m_pool{ other.m_pool }
		{
			m_alloc_count = 0;
		}
		template <typename U>
		allocator& operator=(allocator<U>&& other) noexcept
		{
			if (this == &other)
				return *this;

			AGL_ASSERT(m_pool == nullptr || *this == other, "allocators are not equal");

			m_pool = other.m_pool;
			m_alloc_count = other.m_alloc_count;
			other.m_alloc_count = 0;
			return *this;
		}
		template <typename U> 
		allocator& operator=(allocator<U> const& other) noexcept
		{
			if (this == &other)
				return *this;

			AGL_ASSERT(m_pool == nullptr || *this == other, "allocators are not equal");

			m_alloc_count = 0;
			m_pool = other.m_pool;
			return *this;
		}
		~allocator() noexcept
		{
			AGL_ASSERT(m_alloc_count == 0, "some memory was not deallocated");
		}
		[[nodiscard]] pointer allocate(size_type count = 1, std::uint64_t alignment = alignof(value_type)) noexcept
		{
			AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");

			m_alloc_count += count;
			return reinterpret_cast<T*>(m_pool->allocate(count * sizeof(T), alignment));
		}
		template <typename... TArgs>
		void construct(pointer buffer, TArgs&&... args) noexcept
		{
			AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");
			AGL_ASSERT(buffer != nullptr, "buffer handle is nullptr");
			AGL_ASSERT(m_alloc_count > 0, "invalid construction call");

			new (buffer) T(std::forward<TArgs>(args)...);
		}
		void deallocate(pointer ptr, size_type count = 1) noexcept
		{
			AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");
			AGL_ASSERT(m_alloc_count > 0, "invalid deallocation call");

			m_alloc_count -= count;
			m_pool->deallocate(reinterpret_cast<std::byte*>(ptr), count * sizeof(T));
		}
		void destruct(pointer ptr) noexcept
		{
			AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");
			AGL_ASSERT(ptr != nullptr, "ptr handle is nullptr");
			AGL_ASSERT(m_alloc_count > 0, "invalid destruction call");
			
			ptr->~T();
		}

	private:
		template <typename U>
		friend class allocator;

		template <typename U, typename W>
		friend bool operator==(allocator<U> const&, allocator<W> const&) noexcept;

		template <typename U, typename W>
		friend bool operator!=(allocator<U> const&, allocator<W> const&) noexcept;

	private:
		std::uint64_t m_alloc_count;
		pool* m_pool;
	};

public:
	pool() noexcept
		: resource<pool>{ type_id<pool>::get_id() }
		, m_buffer{ nullptr }
		, m_occupancy{ 0 }
		, m_size{ 0 }
	{
	}
	pool(pool&& other) noexcept
		: resource<pool>{ std::move(other) }
		, m_allocator{ std::move(other.m_allocator) }
		, m_buffer{ other.m_buffer }
		, m_free_spaces{ std::move(other.m_free_spaces) }
		, m_occupancy{ other.m_occupancy }
		, m_size{ other.m_size }
	{
		other.m_buffer = nullptr;
	}
	pool& operator=(pool&& other) noexcept
	{
		if (this == &other)
			return *this;

		this->resource<pool>::operator=(std::move(other));
		m_allocator = std::move(other.m_allocator);
		m_buffer = other.m_buffer;
		other.m_buffer = nullptr;
		m_free_spaces = std::move(other.m_free_spaces);
		m_occupancy = other.m_occupancy;
		m_size = other.m_size;
		return *this;
	}
	~pool() noexcept
	{
		destroy();
	}
	std::byte* allocate(std::uint64_t size, std::uint64_t alignment) noexcept
	{
		AGL_ASSERT(alignment > 0, "Invalid alignment value");
		AGL_ASSERT(size <= this->size(), "Not enough memory to store object");
		AGL_ASSERT(size <= this->size() - occupancy(), "Ran out of memory");

		m_occupancy += size;
		auto* ptr = m_free_spaces.pop(size);
		auto i = size;
		std::align(alignment, size, reinterpret_cast<void*&>(ptr), i);

		AGL_ASSERT(ptr != nullptr, "Invalid pointer returned");

		return ptr;
	}
	bool create(std::uint64_t size) noexcept
	{
		if (m_buffer != nullptr)
			destroy();

		m_buffer = m_allocator.allocate(size);
		m_size = size;

		m_free_spaces.push(m_buffer, m_size);

		return m_buffer != nullptr;
	}
	void destroy() noexcept
	{
		if (m_buffer == nullptr)
			return;

		AGL_ASSERT(empty(), "Some objects were not deallocated");

		m_allocator.deallocate(m_buffer, m_size);
		m_buffer = nullptr;
		m_free_spaces = impl::free_space_tracker{};
		m_occupancy = 0;
		m_size = 0;
	}
	void deallocate(std::byte* ptr, std::uint64_t size) noexcept
	{
		AGL_ASSERT(ptr == nullptr || has_pointer(ptr), "Invalid pointer");
		AGL_ASSERT(!empty(), "pool is empty");
		AGL_ASSERT(occupancy() >= size, "Invalid pointer size");

		m_free_spaces.push(ptr, size);
		m_occupancy -= size;
	}
	bool full() const noexcept
	{
		return occupancy() == size();
	}
	bool empty() const noexcept
	{
		return occupancy() == 0;
	}
	template <typename T>
	allocator<T> make_allocator() noexcept
	{
		return pool::allocator<T>{ this };
	}
	std::uint64_t occupancy() const noexcept
	{
		return m_occupancy;
	}
	std::uint64_t size() const noexcept
	{
		return m_size;
	}
	bool has_pointer(std::byte* ptr) const noexcept
	{
		return m_buffer <= ptr && ptr < m_buffer + size();
	}

private:
	virtual void on_attach(application* app) noexcept override 
	{
		auto& log = app->get_resource<agl::logger>();
		log.info("Pool {} bytes at {}: OK", size(), m_buffer);
	}
	virtual void on_detach(application* app) noexcept override
	{
		auto& log = app->get_resource<agl::logger>();
		log.info("Pool {} bytes at {}: OFF", size(), m_buffer);
	}
	virtual void on_update(application*) noexcept override 
	{
	}

private:
	agl::mem::allocator<std::byte> m_allocator;
	std::byte* m_buffer;
	impl::free_space_tracker m_free_spaces;
	std::uint64_t m_occupancy;
	std::uint64_t m_size;
};

template <typename U, typename W>
bool operator==(pool::allocator<U> const& lhs, pool::allocator<W> const& rhs) noexcept
{
	return lhs.m_pool == rhs.m_pool;
}

template <typename U, typename W>
bool operator!=(pool::allocator<U> const& lhs, pool::allocator<W> const& rhs) noexcept
{
	return lhs.m_pool != rhs.m_pool;
}
}
}