#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
namespace impl
{
	void defragmented_space::push(std::byte* ptr, std::uint64_t size) noexcept
	{
		static auto const comparator = [](membuf const& data, std::uint64_t const size) { return data.size < size; };
		
		// merge adjacent spaces
		auto it = m_defragmented.cbegin();
		for (it; it != m_defragmented.cend(); ++it)
			if (ptr + size == it->ptr || it->ptr + it->size == ptr)
			{
				ptr = std::min(ptr, it->ptr);
				size += it->size;
			}

		auto const found = std::lower_bound(m_defragmented.begin(), m_defragmented.end(), size, comparator);
		m_defragmented.insert(found, membuf{ ptr, size });
	}

	membuf defragmented_space::pop(std::uint64_t minimal_size) noexcept
	{
		static auto const comparator = [](membuf const& data, std::uint64_t const size) { return data.size < size; };
		auto found = std::lower_bound(m_defragmented.begin(), m_defragmented.end(), minimal_size, comparator);

		if (found == m_defragmented.end())
			return membuf{ nullptr, 0 };

		// push excess space back to the collection
		if (found->size > minimal_size)
		{
			push(found->ptr + minimal_size, found->size - minimal_size);
			found->size = minimal_size;
		}

		return *m_defragmented.erase(found);
	}

	membuf const defragmented_space::operator[](std::uint64_t index) const noexcept
	{
		return m_defragmented.at(index);
	}

	std::uint64_t defragmented_space::count() const noexcept
	{
		return m_defragmented.size();
	}
}

	pool::generic_allocator::generic_allocator(pool* ptr) noexcept
		: m_block{ ptr }
	{
	}

	constexpr pool::generic_allocator::generic_allocator(generic_allocator const& other)
		: m_block{ other.m_block }
	{
	}

	pool::pool() noexcept
		: m_buffer{ nullptr }
		, m_occupancy{ 0 }
		, m_peak{ 0 }
		, m_size{ 0 }
	{}

	pool::~pool() noexcept
	{
		clear();
	}

	std::byte* pool::allocate(std::uint64_t size) noexcept
	{
		auto* ptr = static_cast<std::byte*>(nullptr);

		if (m_defragmented.count() > 0)
		{
			auto const s = m_defragmented.pop(size);
		
			if(s.ptr != nullptr)
				ptr = s.ptr;
		}

		m_peak += size;
		m_occupancy += size;
		return m_buffer + m_peak - size;
	}

	bool pool::create(std::uint64_t size) noexcept
	{
		m_buffer = reinterpret_cast<std::byte*>(std::malloc(size));
		return m_buffer != nullptr;
	}

	void pool::deallocate(std::byte* ptr, std::uint64_t size) noexcept
	{
		m_defragmented.push(ptr, size);
		m_occupancy -= size;
	}

	void pool::clear() noexcept
	{
		std::free(m_buffer);
		m_buffer = nullptr;
		m_occupancy = 0;
		m_peak = 0;
		m_size = 0; 
	}

	pool::generic_allocator pool::make_generic_allocator() noexcept
	{
		return generic_allocator{ this };
	}


	std::uint64_t pool::occupancy() const noexcept
	{
		return m_occupancy;
	}

	std::uint64_t pool::size() const noexcept
	{
		return m_size;
	}


	bool operator==(pool::generic_allocator const& lhs, pool::generic_allocator const& rhs) noexcept
	{
		return lhs.m_block == rhs.m_block;
	}

	bool operator!=(pool::generic_allocator const& lhs, pool::generic_allocator const& rhs) noexcept
	{
		return lhs.m_block != rhs.m_block;
	}
}
}
