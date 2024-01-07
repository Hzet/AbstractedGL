#pragma once
#include <cstdint>
#include "agl/core/debug.hpp"
#include "agl/util/memory/pool.hpp"
#include "agl/util/memory/containers.hpp"

namespace agl
{
namespace ecs
{
namespace impl
{
	// deque
	template <typename T>
	class component_buffer
	{
	public:
		class block
		{
		public:
			block(mem::pool::allocator<T> const& allocator = mem::pool::allocator<T>{}) noexcept
				: m_allocator{ allocator }
				, m_data{ nullptr }
				, m_occupancy{ 0 }
				, m_size{ 0 }
			{
			}

			block(block&& other) noexcept
				: m_data{ other.m_data }
				, m_occupancy{ other.m_occupancy }
				, m_size{ other.m_size }
			{
			}

			block& operator=(block&& other) noexcept
			{
				m_allocator = other.m_allocator;
				m_data = other.m_data;
				m_occupancy = other.m_occupancy;
				m_size = other.m_size;
			}

			~block() noexcept
			{
				clear();
			}

			T* const begin() noexcept
			{
				return m_data;
			}

			T const* const cbegin() const noexcept
			{
				return m_data;
			}

			T* const end() noexcept
			{
				return m_data + m_size;
			}

			T const* const cend() const noexcept
			{
				return m_data + m_size;
			}
			
			void create(std::uint64_t block_size) noexcept
			{
				if (!empty())
					clear();

				m_block_size = block_size;
				m_data = reinterpret_cast<T*>(m_allocator.allocate(this->block_size()));
			}

			void clear() noexcept
			{
				if (m_data == nullptr)
					return;

				for (auto i = 0; i < block_size(); ++i)
					m_allocator.destruct(m_data + i);

				m_allocator.deallocate(m_data, block_size());

				m_block_size = 0;
				m_data = nullptr;
				m_free_indexes.clear();
				m_peak = 0;
				m_size = 0;
			}

			bool empty() const noexcept
			{
				return m_size == 0;
			}
			 
			bool full() const noexcept
			{
				return size() == block_size();
			}

			std::uint64_t size() const noexcept
			{
				return m_size;
			}

			std::uint64_t block_size() const noexcept
			{
				return m_block_size;
			}

			template <typename... TArgs>
			T* push(TArgs&&... args) noexcept
			{
				AGL_ASSERT(!full(), "push to a full block");

				auto index = std::uint64_t{};
				if (!m_free_indexes.empty())
					index = *m_free_indexes.erase(m_free_indexes.cend() - 1);
				else
					index = m_peak++;

				++m_size;
				m_allocator.construct(m_data + index, std::forward<TArgs>(args...));
				return m_data + index;
			}

			void pop(T* ptr) noexcept
			{
				AGL_ASSERT(!empty(), "pop on an empty block");

				auto const index = (ptr - m_data) / sizeof(T);

				if (index == m_peak)
					--m_peak;
				else
					m_free_indexes.push_back(index);

				--m_size;
				m_allocator.destruct(ptr);
			}

			T& operator[](std::uint64_t index) noexcept
			{
				AGL_ASSERT(index < m_size, "index out of bounds");

				return *(m_data + index);
			}

			T const& operator[](std::uint64_t index) const noexcept
			{
				AGL_ASSERT(index < m_size, "index out of bounds");

				return *(m_data + index);
			}

		private:
			mem::pool::allocator<T> m_allocator;
			T* m_data;
			mem::vector<std::uint64_t> m_free_indexes;
			std::uint64_t m_size;
			std::uint64_t m_peak;
			std::uint64_t m_block_size;
		};
	
	public:
		component_buffer(mem::pool::allocator<T> const& allocator, std::uint64_t block_size = 1024) noexcept
			: m_allocator{ allocator }
			, m_size{ 0 }
			, m_block_size{ block_size }
		{
		}

		component_buffer(component_buffer&&) noexcept = default;
		component_buffer(component_buffer const&) noexcept = delete;
		component_buffer& operator=(component_buffer&&) noexcept = default;
		component_buffer& operator=(component_buffer const&) noexcept = delete;
		~component_buffer() noexcept = default;

		template <typename... TArgs>
		T* push(TArgs&&... args) noexcept
		{
			++m_size;
			for (auto& block : m_blocks)
				if (block.size() != block.block_size())
					return block.push(std::forward<TArgs>(args...));

			m_blocks.push_back(block{ m_allocator });
			m_blocks.back().create(block_size());
			return m_blocks.back().push(std::forward<TArgs>(args...));
		}

		void pop(T* ptr) noexcept
		{
			for (auto it = m_blocks.cbegin(); it != m_blocks.cend(); ++it)
				if (it->cbegin() <= ptr && ptr <= it->cend())
				{
					--m_size;
					it->pop(ptr);

					if (it->empty())
						m_blocks.erase(it);

					return;
				}
		}

		bool empty() const noexcept
		{
			return m_size == 0;
		}

		std::uint64_t block_size() const noexcept
		{
			return m_block_size;
		}

		std::uint64_t size() const noexcept
		{
			return m_size;
		}

		std::uint64_t block_count() const noexcept
		{
			return m_blocks.size();
		}

		block& operator[](std::uint64_t index) noexcept
		{
			return m_blocks[index];
		}

		block const& operator[](std::uint64_t index) const noexcept
		{
			return m_blocks[index];
		}

	private:
		mem::pool::allocator<T> m_allocator;
		std::uint64_t m_block_size;
		std::uint64_t m_size;
		mem::vector<block> m_blocks;
	};
}
}
}