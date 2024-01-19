#include <gtest/gtest.h>
#include <agl/util/memory/pool.hpp>
#include <agl/util/random.hpp>
#include <agl/util/vector.hpp>
#include <algorithm>

template <typename T>
struct allocation
{
	T* ptr;
	std::uint64_t size;
};

TEST(memory, pool_fill_whole)
{
	auto pool = agl::mem::pool{};
	auto allocator = pool.make_allocator<int>();
	pool.create(sizeof(int) * 10000);

	auto allocs = agl::vector<allocation<int>>{};

	// fill whole 
	while (!pool.full())
	{
		allocs.push_back({ allocator.allocate(), sizeof(int) });
	}
	if (pool.full())
		FAIL() << "Invalid pool occupancy";

	for (auto& v : allocs)
		allocator.deallocate(v.ptr, v.size);

	if (!pool.empty())
		FAIL() << "Invalid pool occupancy / size";

	pool.destroy();
}
TEST(memory, pool_random_allocs)
{

	auto pool = agl::mem::pool{};
	auto allocator = pool.make_allocator<std::byte>();
	auto allocs = agl::vector<allocation<std::byte>>{};
	auto occupancy = 0;

	pool.create(1024 * 1024);
	// push random
	for (auto i = 0; i < 5000; ++i)
	{
		auto size = agl::simple_rand<std::uint64_t>(1, 128);
		allocs.push_back({ allocator.allocate(size), size });
		occupancy += size;
	}

	if (pool.occupancy() != occupancy)
		FAIL() << "Invalid pool occupancy";

	// erase random
	for (auto i = 0; i < 2000; ++i)
	{
		auto index = agl::simple_rand<std::uint64_t>(0, allocs.size() - 1);
		allocator.deallocate(allocs[i].ptr, allocs[i].size);
		allocs.erase(allocs.cbegin() + index);
	}

	// fill till full
	while (!pool.full())
		allocs.push_back({ allocator.allocate(), 1 });

	// sort allocs and check if there are gaps or if allocated objects are overlapping
	std::sort(allocs.begin(), allocs.end(), [](auto const& lhs, auto const& rhs) { return lhs.ptr < rhs.ptr; });
	for (auto it = allocs.cbegin(); it != allocs.cend() - 1; ++it)
	{
		auto next = it++;
		if (it->ptr + it->size > next->ptr)
			FAIL() << "Objects are overlapping: [" << it->ptr << " : " << it->size << "] and [" << next->ptr << " : " << next->size << "]";
		else if(it->ptr + it->size < next->ptr)
			FAIL() << "Free space available but not used between: [" << it->ptr << " : " << it->size << "] and [" << next->ptr << " : " << next->size << "]";
	}

}


/*
TEST(impl_memory_defragmented_space, insertion)
{
	using agl::mem::impl::defragmented_space;
	auto ds = defragmented_space{};
	for (auto i = 0; i < 10000; ++i)
	{
		ds.push(nullptr, agl::simple_rand(1, 128));
		for (auto j = 0; j < ds.count() - 1; ++j)
			EXPECT_TRUE(ds[j].size <= ds[j + 1].size) << "defragmented_space is unsorted";
	}
}
TEST(impl_memory_defragmented_space, deletion)
{
	using agl::mem::impl::defragmented_space;
	auto ds = defragmented_space{};
	auto dummy_addr = std::byte{};
	for (auto i = 0; i < 10000; ++i)
		ds.push(&dummy_addr, agl::simple_rand(1, 128));

	for (auto i = 0; i < ds.count(); ++i)
	{
		auto const rand = agl::simple_rand(1, 130);
		auto const prev_count = ds.count();
		auto const s = ds.pop(rand);

		if (s.ptr == nullptr)
		{
			if(ds.count() != prev_count)
				FAIL() << "item deleted after search has failed";

			for (auto j = 0; j < ds.count(); ++j)
				if(ds[j].size >= rand)
					FAIL() << "space has not been found despite being available to be taken. requested: " << rand << "found: " << ds[j].size;
		}
		else if(s.size < rand)
			FAIL() << "requested: >= " << rand << " got: " << s.size;
	}
}

TEST(impl_memory_defragmented_space, ins_del)
{
	using agl::mem::impl::defragmented_space;
	auto ds = defragmented_space{};
	for (auto i = 0; i < 10000; ++i)
		ds.push(nullptr, agl::simple_rand(1, 128));

	for (auto i = 0; i < 5000; ++i)
	{
		if (agl::simple_rand(0, 2)) // push
			ds.push(nullptr, agl::simple_rand(1, 1024));
		else // pop
			ds.pop(agl::simple_rand(1, 1048));
		
		for(auto j = 0; j < ds.count() - 1; ++j)
			EXPECT_TRUE(ds[j].size <= ds[j + 1].size) << "defragmented_space is unsorted";
	}
}

// TODO: pool tests
*/
