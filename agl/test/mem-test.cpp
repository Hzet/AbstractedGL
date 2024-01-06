#include <gtest/gtest.h>
#include <agl/util/memory/pool.hpp>
#include <agl/util/random.hpp>

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
