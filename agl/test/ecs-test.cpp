#include <gtest/gtest.h>
#include "agl/ecs/ecs.hpp"

TEST(ECS, ecs_storage)
{
	auto pool = agl::mem::pool{};
	pool.create(1024 * 1024 * 1024);

	auto block_size = std::uint64_t{ 1024 };
	auto s = agl::ecs::impl::storage<std::uint64_t>{ pool.make_allocator<std::uint64_t>(), block_size };

	for (auto i = 0; i < 10 * block_size; ++i)
	{
		s.push(i);

		if (s[i] != i)
			FAIL() << "Invalid value after push operation";
	}

	if (s.block_count() != 10)
		FAIL() << "Invalid block count [ 1 ]";

	for(auto i = 0; i < )

}