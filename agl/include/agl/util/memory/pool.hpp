#pragma once
#include <cstddef>
#include "agl/core/debug.hpp"
#include "agl/util/vector.hpp"

namespace agl
{
namespace mem
{
namespace impl 
{
	class membuf
	{
	public:
		using const_iterator = vector<std::byte*>::const_iterator;

	public:
		membuf(std::uint64_t size = 0) noexcept
			: m_space_size{ size }
		{
		}
		const_iterator cbegin() const noexcept
		{
			return m_ptrs.begin();
		}
		const_iterator cend() const noexcept
		{
			return m_ptrs.end();
		}
		void push(std::byte* ptr) noexcept
		{
			auto it = std::lower_bound(m_ptrs.cbegin(), m_ptrs.cend(), ptr);
			m_ptrs.insert(it, ptr);
		}
		std::byte* pop() noexcept
		{
			auto* ptr = m_ptrs.back();
			m_ptrs.erase(m_ptrs.cend() - 1);
			return ptr;
		}
		std::uint64_t space_size() const noexcept
		{
			return m_space_size;
		}
		std::uint64_t empty() const noexcept
		{
			return m_ptrs.empty();
		}
		std::byte const* const front() const noexcept
		{
			return m_ptrs.front();
		}
		std::byte const* const back() const noexcept
		{
			return m_ptrs.back();
		}
		const_iterator find(std::byte* ptr) const noexcept
		{
			if (empty())
				return cend();

			if (front() > ptr || back() > ptr)
				return cend();

			return std::lower_bound(cbegin(), cend(), ptr);
		}
		void erase(const_iterator pos) noexcept
		{
			m_ptrs.erase(pos);
		}
		bool operator<(membuf const& rhs) const noexcept
		{
			return m_space_size < rhs.m_space_size;
		}

	private:
		vector<std::byte*> m_ptrs;
		std::uint64_t m_space_size;
	};

	class defragmented_space
	{
	public:
		/// <summary>
		/// Adds item to the collection. If the space, that the object represents, overlaps with space of another object, the objects will be joined together.
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="size"></param>
		void push(std::byte* ptr, std::uint64_t size) noexcept
		{
			// find and merge adjacent spaces
			auto end_ptr = ptr + size;
			for (auto i = 0; i < m_spaces.size(); ++i)
			{
				auto found = m_spaces[i].find(end_ptr);
				if (found == m_spaces[i].cend())
					continue;

				m_spaces[i].erase(found);
				size += m_spaces[i].space_size();
				end_ptr += m_spaces[i].space_size();
				i = 0;
			}
			// push merged space 
			auto it = std::lower_bound(m_spaces.begin(), m_spaces.end(), size);
			if(it == m_spaces.end())
				it = m_spaces.insert(it, membuf{ size });
			it->push(ptr);
		}

		/// <summary>
		/// Pops item of 'minimal_size' from the collection. If there are only items having more space than requested, 
		/// the object with requested amount of space will be returned and the remaining space pushed back to the collection.
		/// </summary>
		/// <param name="minimal_size"></param>
		/// <returns></returns>
		std::byte* pop(std::uint64_t size) noexcept
		{
			auto it = std::lower_bound(m_spaces.begin(), m_spaces.end(), size);
			
			AGL_ASSERT(it != m_spaces.end(), "No more space available");

			auto* ptr = it->pop();
		}

	private:
		vector<membuf> m_spaces;
	};
}

	class pool
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
			}
			template <typename U> allocator(allocator<U> const& other) noexcept
				: m_pool{ other.m_pool }
			{
			}
			template <typename U> allocator& operator=(allocator<U> const& other) noexcept
			{
				AGL_ASSERT(m_pool == nullptr || *this == other, "allocators are not equal");

				m_pool = other.m_pool;
				return *this;
			}
			~allocator() noexcept = default;
			[[nodiscard]] pointer allocate(size_type count = 1) noexcept
			{
				AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");

				return reinterpret_cast<T*>(m_pool->allocate(count * sizeof(T), alignof(T)));
			}
			template <typename... TArgs>
			void construct(pointer buffer, TArgs&&... args) noexcept
			{
				AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");
				AGL_ASSERT(buffer != nullptr, "buffer handle is nullptr");

				new (buffer) T(std::forward<TArgs>(args)...);
			}
			void deallocate(pointer ptr, size_type count = 1) noexcept
			{
				AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");

				m_pool->deallocate(reinterpret_cast<std::byte*>(ptr), count * sizeof(T));
			}
			void destruct(pointer ptr) noexcept
			{
				AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");
				AGL_ASSERT(ptr != nullptr, "ptr handle is nullptr");

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
			pool* m_pool;
		};

	public:
		pool() noexcept
			: m_buffer{ nullptr }
			, m_occupancy{ 0 }
			, m_size{ 0 }
		{
		}
		pool(pool&&) noexcept = default;
		pool(pool const&) noexcept = delete;
		pool& operator=(pool&&) noexcept = default;
		pool& operator=(pool const&) noexcept = delete;
		~pool() noexcept
		{
			destroy();
		}
		std::byte* allocate(std::uint64_t size, std::uint64_t alignment) noexcept
		{
			m_occupancy += size;
			auto* ptr = m_free_spaces.pop(size);
			auto i = size;
			std::align(alignment, size, reinterpret_cast<void*&>(ptr), i);
			return ptr;
		}
		bool create(std::uint64_t size) noexcept
		{
			if (m_buffer != nullptr)
				destroy();

			m_buffer = reinterpret_cast<std::byte*>(std::malloc(m_size));
			m_size = size;

			m_free_spaces.push(m_buffer, m_size);

			return m_buffer != nullptr;
		}
		void destroy() noexcept
		{
			std::free(m_buffer);
			m_occupancy = 0;
			m_size = 0;
		}
		void deallocate(std::byte* ptr, std::uint64_t size) noexcept
		{
			m_free_spaces.push(ptr, size);
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

	private:
		std::byte* m_buffer;
		impl::defragmented_space m_free_spaces;
		std::uint64_t m_occupancy;
		std::uint64_t m_size;
	};
}
}