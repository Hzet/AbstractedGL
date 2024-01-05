#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
namespace impl
{
	void defragmented_space::push(std::byte* ptr, std::uint64_t size) noexcept
	{
		static auto const comparator = [](memory_block const& data, std::uint64_t const size) { return data.size < size; };
		auto const it = std::lower_bound(m_defragmented.begin(), m_defragmented.end(), size, comparator);
		m_defragmented.insert(it, memory_block{ ptr, size });
	}

	memory_block defragmented_space::pop(std::uint64_t minimal_size) noexcept
	{
		static auto const comparator = [](memory_block const& data, std::uint64_t const size) { return data.size < size; };
		auto const it = std::lower_bound(m_defragmented.begin(), m_defragmented.end(), minimal_size, comparator);

		if (it == m_defragmented.end())
			return memory_block{ nullptr, 0 };
		return *m_defragmented.erase(it);
	}

	memory_block const defragmented_space::operator[](std::uint64_t index) const noexcept
	{
		return m_defragmented.at(index);
	}

	std::uint64_t defragmented_space::count() const noexcept
	{
		return m_defragmented.size();
	}

	block::block() noexcept
		: m_memory{ nullptr }
	{}

	block::~block() noexcept
	{
		destroy();
	}

	std::byte* block::allocate(std::uint64_t size) noexcept
	{
		if (m_defragmented.count() > 0)
		{
			auto const s = m_defragmented.pop(size);
			return s.ptr;
		}

		m_peak += size;
		return m_memory + m_peak - size;
	}

	bool block::create(std::uint64_t size) noexcept
	{
		m_memory = reinterpret_cast<std::byte*>(std::malloc(size));
		return m_memory != nullptr;
	}

	void block::deallocate(std::byte* ptr, std::uint64_t size) noexcept
	{
		m_defragmented.push(ptr, size);
	}

	void block::destroy() noexcept
	{
		std::free(m_memory);
		m_memory = nullptr;
	}

	std::uint64_t block::size() const noexcept
	{
		return m_size;
	}
}
}
}