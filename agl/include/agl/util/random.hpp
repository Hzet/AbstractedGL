#pragma once
#include <cstdint>
#include <atomic>

namespace agl
{
	// https://android.googlesource.com/kernel/common/+/refs/heads/android-4.19-stable/lib/random32.c
	template <typename TInt>
	TInt simple_rand(TInt min, TInt max) noexcept
	{
		struct rnd_state 
		{
			std::atomic<std::uint64_t> s1, s2, s3;
		};

		static auto state = rnd_state{ 4534, 53, 765745 };

	#define TAUSWORTHE(s, a, b, c, d) ((s & c) << d) ^ (((s << a) ^ s) >> b)

		if (min == max)
			return min;
		if (max < min)
			std::swap(max, min);

		state.s1 = TAUSWORTHE(state.s1, 6U, 13U, 4294967294U, 18U);
		state.s2 = TAUSWORTHE(state.s2, 2U, 27U, 4294967288U, 2U);
		state.s3 = TAUSWORTHE(state.s3, 13U, 21U, 4294967280U, 7U);
		
		auto const rand = (state.s1 ^ state.s2 ^ state.s3);
		return rand % (max - min) - min;
	}
}