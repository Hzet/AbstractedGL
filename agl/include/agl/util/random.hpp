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
		static auto state = impl::rnd_state{ 53452u, 432u, 252543u };


		if (min == max)
			return min;
		if (max < min)
			std::swap(max, min);

		state.s1 = TAUSWORTHE(state.s1, 6U, 13U, 4294967294U, 18U);
		state.s2 = TAUSWORTHE(state.s2, 2U, 27U, 4294967288U, 2U);
		state.s3 = TAUSWORTHE(state.s3, 13U, 21U, 4294967280U, 7U);
		
		auto rand = (TNumeric)(state.s1 ^ state.s2 ^ state.s3);
		auto result = max - min;

		if constexpr (std::is_floating_point_v<TNumeric>)
			result = glm::fmod(glm::fabs(rand), glm::fabs(result));
		else if constexpr (std::is_signed_v<TNumeric>)
			result = glm::abs(rand) % glm::abs(result);
		else if constexpr (std::is_unsigned_v<TNumeric>)
			result = rand % result;


		return max - result;
	}
}