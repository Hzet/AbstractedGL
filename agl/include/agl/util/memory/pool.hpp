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
	struct membuf
	{
		std::byte* ptr;
		std::uint64_t size;
	};

	class defragmented_space
	{
	public:
		/// <summary>
		/// Returns number of items held within the object.
		/// </summary>
		/// <returns></returns>
		std::uint64_t count() const noexcept;

		/// <summary>
		/// Adds item to the collection. If the space, that the object represents, overlaps with space of another object, the objects will be joined together.
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="size"></param>
		void push(std::byte* ptr, std::uint64_t size) noexcept;

		/// <summary>
		/// Pops item of 'minimal_size' from the collection. If there are only items having more space than requested, 
		/// the object with requested amount of space will be returned and the remaining space pushed back to the collection.
		/// </summary>
		/// <param name="minimal_size"></param>
		/// <returns></returns>
		membuf pop(std::uint64_t minimal_size) noexcept;

		/// <summary>
		/// Returns item from 'index' position.
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		membuf const operator[](std::uint64_t index) const noexcept;

	private:
		std::vector<membuf> m_defragmented;
	};
}

	class pool
	{
	public:
		template <typename T>
		class allocator
		{
		public:
			using value_type = T;

			allocator(pool* ptr = nullptr) noexcept;
			allocator(allocator&& other);
			allocator& operator=(allocator&& other);
			allocator(allocator const& other);
			allocator& operator=(allocator const& other);
			~allocator() noexcept = default;

			[[nodiscard]] T* allocate(std::uint64_t count = 1) noexcept;

			template <typename... TArgs>
			T* construct(T* buffer, TArgs&&... args) const noexcept;
			void deallocate(T* ptr, std::uint64_t count) noexcept;

			void destruct(T* ptr) const noexcept;
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
		pool() noexcept;
		pool(pool&&) noexcept = default;
		pool(pool const&) noexcept = delete;
		pool& operator=(pool&&) noexcept = default;
		pool& operator=(pool const&) noexcept = delete;
		~pool() noexcept;

		std::byte* allocate(std::uint64_t size) noexcept;		
		bool create(std::uint64_t size) noexcept;
		void clear() noexcept;
		void deallocate(std::byte* ptr, std::uint64_t size) noexcept;
		template <typename T>
		allocator<T> make_allocator() noexcept;
		std::uint64_t occupancy() const noexcept;
		std::uint64_t size() const noexcept;
		
	private:
		std::byte* m_buffer;
		impl::defragmented_space m_defragmented;
		std::uint64_t m_occupancy;
		std::uint64_t m_peak;
		std::uint64_t m_size;
	};

	
#include "agl/util/memory/pool.inl"
}
}