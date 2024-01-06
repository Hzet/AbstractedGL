/*#pragma once
#include <gtest/gtest.h>
#include "agl/util/container/vector.hpp"

TEST(container_test, vector_assign)
{
	auto vec1 = agl::vector<int>{};

	vec1.assign(1000u, 1);

	if (vec1.size() != 500)
		FAIL() << "Invalid size after assign [ 1 ]";

	for (auto const v : vec1)
		if (v != 1)
			FAIL() << "Failed: Alloc vec of 1000 and assign 1 to every field";

	vec1.assign(500, 5);

	if (vec1.size() != 500)
		FAIL() << "Invalid size after assign [ 2 ]";

	for(auto const v : vec1)
		if (v != 5)
			FAIL() << "Failed: Alloc vec of 500 and assign 5 to every field";
	
	auto vec2 = agl::vector<int>{};

	vec2.assign(vec1.cbegin(), vec1.cend());

	if(vec1.size() != vec2.size())
		FAIL() << "Invalid size after assign [ 3 ]";

	for(auto i = 0; i < vec2.size(); ++i)
		if (vec1[i] != vec2[i])
			FAIL() << "Mismatched vector values after copy assignment";
}

TEST(container_test, vector_insert)
{
	auto vec1 = agl::vector<int>{};
}*/