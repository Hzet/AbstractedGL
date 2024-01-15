#include <gtest/gtest.h>
#include "agl/util/vector.hpp"

TEST(vector, vector)
{
	auto vec = agl::vector<int>{};
	
	// empty
	if (!vec.empty())
		FAIL() << "Invalid vector size [ 0 ]";

	// assign
	vec.assign(10000, 5);

	// size
	if(vec.size() != 10000)
		FAIL() << "Invalid vector size [ 1 ]";

	// at 
	for(auto i = 0; i < vec.size(); ++i)
		if(vec.at(i) != 5)
			FAIL() << "Invalid vector value [ 0 ]";

	// []
	for (auto i = 0; i < vec.size(); ++i)
		vec[i] = i;

	auto const_bracket_method = [](agl::vector<int> const& vec)
		{
			for (auto i = 0; i < vec.size(); ++i)
				if (vec[i] != i)
					FAIL() << "Invalid vector value [ const_bracket_method ]";
		};

	const_bracket_method(vec);

	// iterators
	auto count = 0;
	for (auto& v : vec)
	{
		if(v != count)
			FAIL() << "Invalid vector value [ 2 ]";
		++count;
	}

	if(count != 10000)
		FAIL() << "Invalid vector iteration count [ 0 ]";

	count = 0;
	for (auto const& v : vec)
	{
		if (v != count)
			FAIL() << "Invalid vector value [ 3 ]";
		++count;
	}

	if (count != 10000)
		FAIL() << "Invalid vector iteration count [ 1 ]";

	// reverse iterators
	{
		vec.clear();
		vec.reserve(10000);
		for (auto i = 0; i < 10000; ++i)
			vec.push_back(i);

		count = vec.size() - 1;
		auto const rbegin = vec.rbegin();
		auto const rend = vec.rend();
		for (auto it = rbegin; it != rend; ++it)
		{
			if (*it != count)
				FAIL() << "Invalid vector value [ 4 ]";
			--count;
		}

		if (count != 0)
			FAIL() << "Invalid vector iteration count [ 2 ]";
	}

	count = vec.size() - 1;
	for (auto it = vec.crbegin(); it != vec.crend(); ++it)
	{
		if (*it != count)
			FAIL() << "Invalid vector value [ 5 ]";
		--count;
	}

	if (count != 0)
		FAIL() << "Invalid vector iteration count [ 3 ]";

	// clear
	vec.clear();
	if(!vec.empty())
		FAIL() << "Invalid vector size [ 2 ]";

	// reserve 
	vec.reserve(10000);
	if (vec.capacity() < 10000)
		FAIL() << "Invalid vector capacity [ 0 ]";
	if(!vec.empty())
		FAIL() << "Invalid vector size [ 3 ]";

	// push_back
	for (auto i = 0; i < vec.capacity(); ++i)
		vec.push_back(i);

	for (auto i = 0; i < vec.size(); ++i)
		if(vec[i] != i)
			FAIL() << "Invalid vector value [ 6 ]";

	// pop_back
	for (auto i = 0; i < vec.capacity(); ++i)
	{
		vec.pop_back();
		if(vec.size() != 10000 - i - 1)
			FAIL() << "Invalid vector size [ 4 ]";

		if(vec.back() != 10000 - i - 1)
			FAIL() << "Invalid vector value [ 7 ]";
	}
}
