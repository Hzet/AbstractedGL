#pragma once
#include <cstddef>
#include <vector>

#include "agl/core/debug.hpp"

namespace agl
{
namespace mem
{
namespace impl 
{
	struct memory_block
	{
		std::byte* ptr;
		std::uint64_t size;
	};

	class defragmented_space
	{
	public:
		void push(std::byte* ptr, std::uint64_t size) noexcept
		{
			static auto const comparator = [](memory_block const& data, std::uint64_t const size) { return data.size < size; };
			auto const it = std::lower_bound(m_defragmented.begin(), m_defragmented.end(), size, comparator);
			m_defragmented.insert(it, memory_block{ ptr, size });
		}

		memory_block pop(std::uint64_t minimal_size) noexcept
		{
			static auto const comparator = [](memory_block const& data, std::uint64_t const size) { return data.size < size; };
			auto const it = std::lower_bound(m_defragmented.begin(), m_defragmented.end(), minimal_size, comparator);

			if (it == m_defragmented.end())
				return memory_block{ nullptr, 0 };
			return *m_defragmented.erase(it);
		}

		memory_block const operator[](std::uint64_t index) const
		{
			return m_defragmented.at(index);
		}

		std::uint64_t count() const noexcept
		{
			return m_defragmented.size();
		}

	private:
		std::vector<memory_block> m_defragmented;
	};

	class block
	{
	public:
		block() noexcept
			: m_memory{ nullptr }
		{}

		~block() noexcept
		{
			destroy();
		}

		std::byte* allocate(std::uint64_t size) noexcept
		{
			if (m_defragmented.count() > 0)
			{
				auto const s = m_defragmented.pop(size);
				return s.ptr;
			}

			m_peak += size;
			return m_memory + m_peak - size;
		}
		
		bool create(std::uint64_t size)
		{
			m_memory = reinterpret_cast<std::byte*>(std::malloc(size));
		}

		void deallocate(std::byte* ptr, std::uint64_t size) noexcept
		{
			m_defragmented.push(ptr, size);
		}

		void destroy()
		{
			std::free(m_memory);
			m_memory = nullptr;
		}

		std::uint64_t size() const
		{
			return m_size;
		}

	private:
		
	private:
		defragmented_space m_defragmented;
		std::byte* m_memory;
		std::uint64_t m_peak;
		std::uint64_t m_size;
	};
}
}
}