#include "gtest/gtest.h"
#include <tuple>
#include "agl/util/typeid.hpp"
#include <vector>

namespace agl
{
namespace impl
{
	template <typename... TArgs>
	constexpr void is_constexpr(TArgs&&...) noexcept {};
}
}

#define IS_CONSTEXPR(...) noexcept(::agl::impl::is_constexpr(__VA_ARGS__))

TEST(util_type_id, is_constexpr)
{
	static_assert(!IS_CONSTEXPR(std::make_unique<int>())); // control
	static_assert(IS_CONSTEXPR(::agl::type_id<int>::get_id()));
	static_assert(IS_CONSTEXPR(::agl::type_id<int>::get_name()));
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
	EXPECT_EQ(agl::type_id<std::uint64_t>::get_name(), "std::uint64_t");
	EXPECT_EQ(agl::type_id<std::tuple<int>>::get_name(), "std::tuple<int>");
	EXPECT_EQ(agl::type_id<std::tuple<std::tuple<int>>>::get_name(), "std::tuple<std::tuple<int>>");
	EXPECT_EQ(agl::type_id<std::tuple<std::uint64_t, int, int>>::get_name(), "std::tuple<std::uint64_t, int, int>>");
	EXPECT_EQ(agl::type_id<std::tuple<std::tuple<std::uint64_t>, std::tuple<int, int>>>::get_name(), "std::tuple<std::tuple<std::uint64_t>, std::tuple<int, int>>");
}
