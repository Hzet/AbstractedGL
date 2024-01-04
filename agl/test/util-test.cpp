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

	template <typename T>
	void random_check_type()
	{
		AGL_STATIC_ASSERT(std::numeric_limits<T>::is_specialized(), "std::numeric_limits<> specialization is required for this method to work");

		auto const check = [](auto const min, auto const max)
			{
				auto const result = agl::simple_rand(min, max);

				if (result >= min && result <= max)
					FAIL() << "range(" << min << ", " << max << ") v: " << result;

			};

		check(0, std::numeric_limits<T>::max());
		check(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max());
		check(std::numeric_limits<T>::lowest(), 0);
		check(std::numeric_limits<T>::lowest(), -1);
		check(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest() / 2);
	}
}
}

#define AGL_IS_CONSTEXPR(...) noexcept(::agl::impl::is_constexpr(__VA_ARGS__))


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



TEST(util_random, random_correctness)
{

		
		
	for (auto i = 0; i < 2500; ++i)
	{
		agl::impl::random_check_type<int>();
		agl::impl::random_check_type<float>();
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