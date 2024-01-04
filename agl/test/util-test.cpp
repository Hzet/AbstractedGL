#include "gtest/gtest.h"
#include <tuple>
#include "agl/util/typeid.hpp"
#include "agl/util/random.hpp"

namespace agl
{
namespace impl
{
	template <typename... TArgs>
	constexpr void is_constexpr(TArgs&&...) noexcept {};
}
}

#define AGL_IS_CONSTEXPR(...) noexcept(::agl::impl::is_constexpr(__VA_ARGS__))
#define AGL_TO_STR(str) #str

TEST(util_type_id, is_constexpr)
{
	static_assert(!AGL_IS_CONSTEXPR(std::make_unique<int>()), "control statement failed\n" AGL_FILE ":" AGL_TO_STR(AGL_LINE)); // control
	static_assert(AGL_IS_CONSTEXPR(::agl::type_id<int>::get_id()), "agl::type_id::get_id() failed to be evaulated in compile time\n" AGL_FILE ":" AGL_TO_STR(AGL_LINE));
	static_assert(AGL_IS_CONSTEXPR(::agl::type_id<int>::get_name()), "agl::type_id::get_name() failed to be evaluated in compile time\n" AGL_FILE ":" AGL_TO_STR(AGL_LINE));
}

TEST(util_type_id, get_id)
{
	EXPECT_TRUE(agl::type_id<std::uint64_t>::get_id() == agl::type_id<std::uint64_t>::get_id());
	EXPECT_TRUE(agl::type_id<int>::get_id() == agl::type_id<int>::get_id());
	EXPECT_FALSE(agl::type_id<std::string_view>::get_id() == agl::type_id<int>::get_id());
	EXPECT_FALSE(agl::type_id<int>::get_id() == agl::type_id<std::string_view>::get_id());
}

TEST(util_type_id, get_name)
{
	EXPECT_STREQ(std::string{ agl::type_id<std::uint64_t>::get_name() }.c_str(), typeid(std::uint64_t).name());
	EXPECT_STREQ(std::string{ agl::type_id<std::tuple<int>>::get_name() }.c_str(), typeid(std::tuple<int>).name());
	EXPECT_STREQ(std::string{ agl::type_id<std::tuple<std::tuple<int>>>::get_name() }.c_str(), typeid(std::tuple<std::tuple<int>>).name());

	auto const n1 = std::string{ agl::type_id<std::tuple<std::uint64_t, int, int>>::get_name() };
	EXPECT_STREQ(n1.c_str(), typeid(std::tuple<std::uint64_t, int, int>).name());

	auto const n2 = std::string{ agl::type_id<std::tuple<std::tuple<std::uint64_t>, std::tuple<int, int>>>::get_name()};
	EXPECT_STREQ(n2.c_str(), typeid(std::tuple<std::tuple<std::uint64_t>, std::tuple<int, int>>).name());
}

TEST(util_random, random_speed_10k)
{
	for (auto i = 0; i < 10000; ++i)
		agl::simple_rand(0, 1000000);
}

TEST(util_random, random_correctness)
{
	auto const check = [](auto const min, auto const max, auto const v)
		{
			EXPECT_TRUE(v > min) << "range(" << min << ", " << max << ") v: " << v;
			EXPECT_TRUE(v < max) << "range(" << min << ", " << max << ") v: " << v;
		};
	for (auto i = 0; i < 2500; ++i)
	{
		auto const result = agl::simple_rand(0, 100000);

	}
}

TEST(util_random, random_example)
{
	auto map = std::map<std::uint64_t, std::uint64_t>{};
	auto min = 10000;
	auto max = 0;
	for (auto i = 0; i < 10000; ++i)
	{
		auto const rand = agl::simple_rand(0, 10000);
		++map[rand];
	}

	for (auto const& e : map)
	{
		if (min > e.second)
			min = e.second;
		if (max < e.second)
			max = e.second;
	}

	//std::cout << "min: " << min << " max: " << max;
	//for (auto const& e : map)
	//	std::cout << e.first << ": " << e.second << std::endl;
}