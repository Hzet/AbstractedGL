#include <gtest/gtest.h>
#include <agl/memory/pool.hpp>
#include <agl/util/random.hpp>
#include <agl/vector.hpp>
#include <agl/set.hpp>
#include <algorithm>
#include <array>

template <typename T>
struct allocation
{
	T* ptr;
	std::uint64_t size;
};

template <typename T>
bool operator<(allocation<T> const& lhs, allocation<T> const& rhs)
{
	return lhs.ptr < rhs.ptr;
}

TEST(pool, impl_defragmented_space)
{
	auto ds = agl::mem::impl::free_space_tracker{};
	constexpr auto size = 1024 * 10;
	auto arr = std::array<std::byte, size>{};
	auto* begin = &arr[0];
	auto* end = begin + size;
	ds.push(begin, size);

	for (auto i = 0; i < 100; ++i)
	{
		auto rnd_size = agl::simple_rand<std::uint64_t>(0, 128);
		auto* ptr = ds.pop(rnd_size);
		
		if (ptr < begin || ptr >= end)
			FAIL() << "Invalid pointer returned [ 0 ]";
	}

	while (ds.has_space())
	{
		auto* ptr = ds.pop(1);

		if (ptr < begin || ptr >= end)
			FAIL() << "Invalid pointer returned [ 1 ]";
	}
}

TEST(memory, pool_fill_whole)
{
	auto pool = agl::mem::pool{};
	pool.create(sizeof(int) * 10000);

	{ // ensure pool gets destroyed as last
		auto allocator = pool.make_allocator<int>();
		auto allocs = agl::set<allocation<int>>{};
		allocs.reserve(10000);
		// fill whole 
		while (!pool.full())
		{
			auto* ptr = allocator.allocate();
			if (!pool.has_pointer(reinterpret_cast<std::byte*>(ptr)))
				FAIL() << "Invalid pointer returned";

			allocs.emplace({ ptr, sizeof(int) });
		}
		if (!pool.full())
			FAIL() << "Invalid pool occupancy";

		for (auto& v : allocs)
			allocator.deallocate(v.ptr);

		if (!pool.empty())
			FAIL() << "Invalid pool occupancy / size";
	}
}
TEST(memory, pool_random_allocs)
{
	auto pool = agl::mem::pool{};
	pool.create(1024 * 100);

	{ // ensure pool gets destroyed as last
		auto allocator = pool.make_allocator<std::byte>();
		auto allocs = agl::set<allocation<std::byte>>{};
		auto occupancy = 0;

		
		allocs.reserve(5000);
		// push random
		for (auto i = 0; i < 2500; ++i)
		{
			auto size = agl::simple_rand<std::uint64_t>(1, 32);
			auto* ptr = allocator.allocate(size);

			if (!pool.has_pointer(reinterpret_cast<std::byte*>(ptr)))
				FAIL() << "Invalid pointer returned";

			allocs.emplace({ ptr, size });
			occupancy += size;
		}

		if (pool.occupancy() != occupancy)
			FAIL() << "Invalid pool occupancy";

		// erase random
		for (auto i = 0; i < 200; ++i)
		{
			auto index = agl::simple_rand<std::uint64_t>(0, allocs.size() - 1);
			auto it = allocs.cbegin() + index;
			allocator.deallocate(it->ptr, it->size);
			allocs.erase(it);
		}

		// fill till full
		allocs.reserve(80000);
		while (!pool.full())
		{
			auto* ptr = allocator.allocate();

			if (!pool.has_pointer(reinterpret_cast<std::byte*>(ptr)))
				FAIL() << "Invalid pointer returned";
			allocs.emplace({ ptr, 1 });
		}

		// sort allocs and check if there are gaps or if allocated objects are overlapping
		std::sort(allocs.begin(), allocs.end(), [](auto const& lhs, auto const& rhs) { return lhs.ptr < rhs.ptr; });
		auto overlapping = 0;
		auto unused_memory = 0;
		for (auto it = allocs.cbegin(); it != allocs.cend() - 1; ++it)
		{
			auto next = it++;
			if (it->ptr + it->size > next->ptr)
				++overlapping;
			else if (it->ptr + it->size < next->ptr)
				++unused_memory;
		}

		if (overlapping > 0 || unused_memory > 0)
			FAIL() << "Overlapping: " << overlapping << " unused memory: " << unused_memory;

		for (auto& alloc : allocs)
			allocator.deallocate(alloc.ptr, alloc.size);
	}
}