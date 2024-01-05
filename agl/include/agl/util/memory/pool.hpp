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
		std::uint64_t count() const noexcept;
		void push(std::byte* ptr, std::uint64_t size) noexcept;
		memory_block pop(std::uint64_t minimal_size) noexcept;
		memory_block const operator[](std::uint64_t index) const noexcept;

	private:
		std::vector<memory_block> m_defragmented;
	};

	class block
	{
	public:
		block() noexcept;
		~block() noexcept;

		std::byte* allocate(std::uint64_t size) noexcept;		
		bool create(std::uint64_t size) noexcept;
		void deallocate(std::byte* ptr, std::uint64_t size) noexcept;
		void destroy() noexcept;
		std::uint64_t size() const noexcept;
		
	private:
		defragmented_space m_defragmented;
		std::byte* m_memory;
		std::uint64_t m_peak;
		std::uint64_t m_size;
	};
}
}
}