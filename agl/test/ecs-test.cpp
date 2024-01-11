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
}

TEST(ECS, ecs)
{
	auto pool = agl::mem::pool{};
	pool.create(1024 * 1024 * 1024);

	auto allocator = pool.make_allocator<int>();

	auto ecs = agl::ecs::organizer{ allocator };

	auto entities = agl::mem::vector<agl::ecs::entity>{ allocator };
	entities.reserve(1000);

	for (auto i = 0; i < entities.capacity(); ++i)
		entities.push_back(ecs.make_entity());

	for (auto i = 0; i < entities.size(); ++i)
	{
		ecs.push_component<int>(entities[i], i);
		ecs.push_component<int>(entities[i], i + 1);
		ecs.push_component<float>(entities[i], i);
	}

	for (auto i = 0; i < entities.size(); ++i)
	{
		if (entities[i].size<int>() != 2)
			FAIL() << "Invalid component count size [ 0 ]";

		if (entities[i].get_component<int>(0) != i)
			FAIL() << "Invalid value after insertion [ 0 ]";

		if (entities[i].get_component<int>(1) != i + 1)
			FAIL() << "Invalid value after insertion [ 1 ]";

		if (entities[i].size<float>() != 1)
			FAIL() << "Invalid component count size [ 1 ]";

		if (entities[i].get_component<float>(0) != i)
			FAIL() << "Invalid value after insertion [ 2 ]";
	}


}	