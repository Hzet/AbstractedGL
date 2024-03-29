#pragma once
#include <cstdint>
#include <cmath>
#include <glm/glm.hpp>

#define TAUSWORTHE(s, a, b, c, d) ((s & c) << d) ^ (((s << a) ^ s) >> b)

namespace agl
{
	// https://android.googlesource.com/kernel/common/+/refs/heads/android-4.19-stable/lib/random32.c
namespace impl
{
	struct rnd_state
	{
		std::uint64_t s1;
		std::uint64_t s2;
		std::uint64_t s3;
	};
}

	template <typename TNumeric>
	TNumeric simple_rand(TNumeric min, TNumeric max)
	{
		if constexpr (!std::is_floating_point_v<TNumeric>)
			return std::rand() % (max - min) + min;

		auto rand = std::rand() % static_cast<int>(std::numeric_limits<TNumeric>::max());
		auto result = TNumeric{};

		for (auto i = 1; i < std::numeric_limits<TNumeric>::max(); i *= 10)
			if (i > rand)
			{
				result = rand / static_cast<float>(i);
				break;
			}

		if (static_cast<int>(max - min - 1) != 0)
			result += std::rand() % static_cast<int>(max - min - 1) + min;;

		return result;
	}
	/*
	template <typename TNumeric>
	TNumeric simple_rand(TNumeric min, TNumeric max)
	{
		static auto state = impl::rnd_state{ 53452u, 432u, 252543u };

		if (min == max)
			return min;
		if (max < min)
			std::swap(max, min);

		state.s1 = TAUSWORTHE(state.s1, 6U, 13U, 4294967294U, 18U);
		state.s2 = TAUSWORTHE(state.s2, 2U, 27U, 4294967288U, 2U);
		state.s3 = TAUSWORTHE(state.s3, 13U, 21U, 4294967280U, 7U);
		
		auto rand = *(TNumeric*)(state.s1 ^ state.s2 ^ state.s3);
		auto count = max - min;
		auto result = TNumeric{};

		if constexpr (std::is_floating_point_v<TNumeric>)
			result = glm::mod(rand, count);
		else if constexpr (std::is_signed_v<TNumeric>)
			result = glm::abs(rand) % glm::abs(count);
		else if constexpr (std::is_unsigned_v<TNumeric>)
			result = rand % result;


		return result;
	}
	*/
}