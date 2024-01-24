#include <gtest/gtest.h>
#include <vector>

#include "agl/util/vector.hpp"
#include "agl/util/deque.hpp"
#include "agl/util/set.hpp"
#include "agl/util/random.hpp"
#include "agl/util/dictionary.hpp"

auto const size = 100000;

TEST(vector, vector)
{
	auto vec = agl::vector<int>{};
	auto std_vec = std::vector<int>{};
	// empty
	if (!vec.empty())
		FAIL() << "Invalid vector size [ 0 ]";

	// assign
	vec.assign(size, 5);

	// size
	if(vec.size() != size)
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

	if(count != size)
		FAIL() << "Invalid vector iteration count [ 0 ]";

	count = 0;
	for (auto const& v : vec)
	{
		if (v != count)
			FAIL() << "Invalid vector value [ 3 ]";
		++count;
	}

	if (count != size)
		FAIL() << "Invalid vector iteration count [ 1 ]";

	// reverse iterators
	vec.clear();
	vec.reserve(size);
	for (auto i = 0; i < size; ++i)
		vec.push_back(i);

	count = vec.size();
	for (auto it = vec.rbegin(); it != vec.rend(); ++it)
	{
		if (*it != count - 1)
			FAIL() << "Invalid vector value [ 4 ]";
		--count;
	}

	if (count != 0)
		FAIL() << "Invalid vector iteration count [ 2 ]";

	count = vec.size();
	for (auto it = vec.crbegin(); it != vec.crend(); ++it)
	{
		if (*it != count - 1)
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
	vec.reserve(size);
	if (vec.capacity() < size)
		FAIL() << "Invalid vector capacity [ 0 ]";
	if(!vec.empty())
		FAIL() << "Invalid vector size [ 3 ]";

	// push_back
	for (auto i = 0; i < size; ++i)
		vec.push_back(i);

	for (auto i = 0; i < vec.size(); ++i)
		if(vec[i] != i)
			FAIL() << "Invalid vector value [ 6 ]";

	// pop_back
	for (auto i = 0; i < size; ++i)
	{
		if(vec.size() != size - i)
			FAIL() << "Invalid vector size [ 4 ]";

		auto expected = size - i - 1;
		if(vec.back() != expected)
			FAIL() << "Invalid vector value [ 7 ]";

		vec.pop_back();
	}

	vec.reserve(size);
	std_vec.reserve(size);
	for (auto i = 0; i < size; ++i)
	{
		std_vec.push_back(i);
		vec.push_back(i);
	}

	while (!vec.empty())
	{
		auto index = agl::simple_rand(std::uint64_t{}, vec.size() - 1);
		vec.erase(vec.cbegin() + index);
		std_vec.erase(std_vec.cbegin() + index);

		if(std_vec.size() != vec.size())
			FAIL() << "Invalid vector size [ 5 ]";

		for(auto i = 0; i < std_vec.size(); ++i)
			if(vec[i] != std_vec[i])
				FAIL() << "Invalid vector value [ 8 ]";
	}
}

TEST(vector, vector_ptr)
{
	auto vec = agl::vector<int*>{};
	auto integer = 0;

	// empty
	if (!vec.empty())
		FAIL() << "Invalid vector size [ 0 ]";

	// assign
	vec.assign(size, nullptr);

	// size
	if (vec.size() != size)
		FAIL() << "Invalid vector size [ 1 ]";

	// at 
	for (auto i = 0; i < vec.size(); ++i)
		if (vec.at(i) != nullptr)
			FAIL() << "Invalid vector value [ 0 ]";

	// []
	for (auto i = 0; i < vec.size(); ++i)
		vec[i] = &integer;

	auto const_bracket_method = [&](agl::vector<int*> const& vec)
		{
			for (auto i = 0; i < vec.size(); ++i)
				if (vec[i] != &integer)
					FAIL() << "Invalid vector value [ const_bracket_method ]";
		};

	const_bracket_method(vec);

	// iterators
	auto count = 0;
	for (auto& v : vec)
	{
		if (v != &integer)
			FAIL() << "Invalid vector value [ 2 ]";
		++count;
	}

	if (count != size)
		FAIL() << "Invalid vector iteration count [ 0 ]";

	count = 0;
	for (auto const v : vec)
	{
		if (v != &integer)
			FAIL() << "Invalid vector value [ 3 ]";
		++count;
	}

	if (count != size)
		FAIL() << "Invalid vector iteration count [ 1 ]";

	// reverse iterators
	vec.clear();
	vec.reserve(size);
	for (auto i = 0; i < size; ++i)
		vec.push_back(nullptr);

	count = vec.size();
	for (auto it = vec.rbegin(); it != vec.rend(); ++it)
	{
		if (*it != nullptr)
			FAIL() << "Invalid vector value [ 4 ]";
		--count;
	}

	if (count != 0)
		FAIL() << "Invalid vector iteration count [ 2 ]";

	count = vec.size();
	for (auto it = vec.crbegin(); it != vec.crend(); ++it)
	{
		if (*it != nullptr)
			FAIL() << "Invalid vector value [ 5 ]";
		--count;
	}

	if (count != 0)
		FAIL() << "Invalid vector iteration count [ 3 ]";

	// clear
	vec.clear();
	if (!vec.empty())
		FAIL() << "Invalid vector size [ 2 ]";

	// reserve 
	vec.reserve(size);
	if (vec.capacity() < size)
		FAIL() << "Invalid vector capacity [ 0 ]";
	if (!vec.empty())
		FAIL() << "Invalid vector size [ 3 ]";

	// push_back
	for (auto i = 0; i < size; ++i)
		vec.push_back(nullptr);

	for (auto i = 0; i < vec.size(); ++i)
		if (vec[i] != nullptr)
			FAIL() << "Invalid vector value [ 6 ]";

	// pop_back
	for (auto i = 0; i < size; ++i)
	{
		if (vec.size() != size - i)
			FAIL() << "Invalid vector size [ 4 ]";

		if (vec.back() != nullptr)
			FAIL() << "Invalid vector value [ 7 ]";

		vec.pop_back();
	}
}

TEST(deque, deque)
{
	auto deq = agl::deque<int>{};

	// empty
	if (!deq.empty())
		FAIL() << "Invalid deque size [ 0 ]";

	// push_back
	for (auto i = 0; i < size; ++i)
		deq.push_back(i);

	// size
	if (deq.size() != size)
		FAIL() << "Invalid deque size [ 1 ]";

	// at 
	for (auto i = 0; i < deq.size(); ++i)
		if (deq.at(i) != i)
			FAIL() << "Invalid deque value [ 0 ]";

	// []
	for (auto i = 0; i < deq.size(); ++i)
		deq[i] = i;

	auto const_bracket_method = [](agl::deque<int> const& vec)
		{
			for (auto i = 0; i < vec.size(); ++i)
				if (vec[i] != i)
					FAIL() << "Invalid deque value [ const_bracket_method ]";
		};

	const_bracket_method(deq);

	// iterators
	auto count = 0;
	for (auto& v : deq)
	{
		if (v != count)
			FAIL() << "Invalid deque value [ 2 ]";
		++count;
	}

	if (count != size)
		FAIL() << "Invalid deque iteration count [ 0 ]";

	count = 0;
	for (auto const& v : deq)
	{
		if (v != count)
			FAIL() << "Invalid deque value [ 3 ]";
		++count;
	}

	if (count != size)
		FAIL() << "Invalid deque iteration count [ 1 ]";

	// reverse iterators
	deq.clear();
	for (auto i = 0; i < size; ++i)
		deq.push_back(i);

	count = deq.size();
	for (auto it = deq.rbegin(); it != deq.rend(); ++it)
	{
		if (*it != count - 1)
			FAIL() << "Invalid deque value [ 4 ]";
		--count;
	}

	if (count != 0)
		FAIL() << "Invalid deque iteration count [ 2 ]";

	count = deq.size();
	for (auto it = deq.crbegin(); it != deq.crend(); ++it)
	{
		if (*it != count - 1)
			FAIL() << "Invalid deque value [ 5 ]";
		--count;
	}

	if (count != 0)
		FAIL() << "Invalid deque iteration count [ 3 ]";

	// clear
	deq.clear();
	if (!deq.empty())
		FAIL() << "Invalid deque size [ 2 ]";

	// push_back
	for (auto i = 0; i < size; ++i)
		deq.push_back(i);

	for (auto i = 0; i < deq.size(); ++i)
		if (deq[i] != i)
			FAIL() << "Invalid deque value [ 6 ]";

	for (auto i = 0; i < size; ++i)
	{
		if (deq.size() != size - i)
			FAIL() << "Invalid deque size [ 3 ]";

		auto expected = size - i - 1;
		if (deq.back() != expected)
			FAIL() << "Invalid deque value [ 7 ]" << "deq[" << i << "]: " << deq.back() << " expected: " << expected;

		auto idx = deq.cend() - 1;
		deq.erase(deq.cend() - 1 );
	}
}


TEST(deque, deque_ptr)
{
	auto deq = agl::deque<int*>{};

	// empty
	if (!deq.empty())
		FAIL() << "Invalid deque size [ 0 ]";

	// push_back
	for (auto i = 0; i < size; ++i)
		deq.push_back(nullptr);

	// size
	if (deq.size() != size)
		FAIL() << "Invalid deque size [ 1 ]";

	// at 
	for (auto i = 0; i < deq.size(); ++i)
		if (deq.at(i) != nullptr)
			FAIL() << "Invalid deque value [ 0 ]";

	// []
	int address = {};
	for (auto i = 0; i < deq.size(); ++i)
		deq[i] = &address;

	auto const_bracket_method = [&](agl::deque<int*> const& vec)
		{
			for (auto i = 0; i < vec.size(); ++i)
				if (vec[i] != &address)
					FAIL() << "Invalid deque value [ const_bracket_method ]";
		};

	const_bracket_method(deq);

	// iterators
	auto count = 0;
	for (auto v : deq)
	{
		if (v != &address)
			FAIL() << "Invalid deque value [ 2 ]";
		++count;
	}

	if (count != size)
		FAIL() << "Invalid deque iteration count [ 0 ]";

	count = 0;
	for (auto const& v : deq)
	{
		if (v != &address)
			FAIL() << "Invalid deque value [ 3 ]";
		++count;
	}

	if (count != size)
		FAIL() << "Invalid deque iteration count [ 1 ]";

	// reverse iterators
	deq.clear();
	for (auto i = 0; i < size; ++i)
		deq.push_back(&address);

	count = deq.size();
	for (auto it = deq.rbegin(); it != deq.rend(); ++it)
	{
		if (*it != &address)
			FAIL() << "Invalid deque value [ 4 ]";
		--count;
	}

	if (count != 0)
		FAIL() << "Invalid deque iteration count [ 2 ]";

	count = deq.size();
	for (auto it = deq.crbegin(); it != deq.crend(); ++it)
	{
		if (*it != &address)
			FAIL() << "Invalid deque value [ 5 ]";
		--count;
	}

	if (count != 0)
		FAIL() << "Invalid deque iteration count [ 3 ]";

	// clear
	deq.clear();
	if (!deq.empty())
		FAIL() << "Invalid deque size [ 2 ]";

	// push_back
	for (auto i = 0; i < size; ++i)
		deq.push_back(nullptr);

	for (auto i = 0; i < deq.size(); ++i)
		if (deq[i] != nullptr)
			FAIL() << "Invalid deque value [ 6 ]";

	for (auto i = 0; i < size; ++i)
	{
		if (deq.size() != size - i)
			FAIL() << "Invalid deque size [ 3 ]";

		if (deq.back() != nullptr)
			FAIL() << "Invalid deque value [ 7 ]";

		deq.erase(deq.cbegin() + deq.size() - 1);
	}
}

TEST(dictionary, dictionary)
{
	auto const size = 100000;
	auto dict = agl::dictionary<int, int>{};

	// emplace 
	for (auto i = 0; i < size; ++i)
		dict.emplace(i, i);

	// size
	if (dict.size() != size)
		FAIL() << "Invalid dictionary size [ 0 ]";

	// []
	for (auto i = 0; i < dict.size(); ++i)
		if (dict[i] != i)
			FAIL() << "Invalid dictionary value [ 0 ]";

	// clear
	dict.clear();

	// empty
	if(!dict.empty())
		FAIL() << "Invalid dictionary size [ 1 ]";

	// [] emplace
	for (auto i = 0; i < size; ++i)
		dict[i] = i;

	for (auto i = 0; i < dict.size(); ++i)
		if (dict.at(i) != i)
			FAIL() << "Invalid dictionary value [ 1 ]";

	// erase
	auto arr = agl::vector<bool>{};
	arr.resize(size);

	for (auto i = 0; i < size; ++i)
		arr[i] = true;

	for (auto it = dict.cbegin() + 1; it != dict.cend(); ++it)
		if ((it - 1)->first > it->first)
			FAIL() << "dictionary is unsorted [ 0 ]";

	auto index = agl::simple_rand(0, size);
	while (!dict.size() > 100)
	{
		while (!arr[index])
			index = agl::simple_rand(0, 9999);

		arr[index] = false;

		dict.erase(dict.cbegin() + index);

		if (dict.find(index) != dict.end())
			FAIL() << "Invalid dictionary erase algorithm [ 0 ]";
	}

	while (dict.size() < size)
	{
		auto rnd = agl::simple_rand(std::numeric_limits<std::int64_t>::lowest(), std::numeric_limits<std::int64_t>::max());
		
		if (dict.find(rnd) != dict.end())
			continue;

		dict.emplace(rnd, rnd);
	}

	for (auto it = dict.cbegin() + 1; it != dict.cend(); ++it)
		if ((it - 1)->first > it->first)
			FAIL() << "dictionary is unsorted [ 1 ]";
}

TEST(set, set)
{
	struct foo 
	{
		int x; 
		operator int()  { return x; }
	};
	
	struct foo_comp { bool operator()(foo l, foo r) const noexcept { return l.x < r.x; } };

	auto const size = 100000;
	auto set = agl::set<foo, foo_comp>{};

	// emplace 
	for (auto i = 0; i < size; ++i)
		set.emplace({ i });

	// size
	if (set.size() != size)
		FAIL() << "Invalid set size [ 0 ]";

	// at
	for (auto i = 0; i < set.size(); ++i)
		if (set.at({ i }) != i)
			FAIL() << "Invalid set value [ 0 ]";

	// clear
	set.clear();

	// empty
	if (!set.empty())
		FAIL() << "Invalid dictionary size [ 1 ]";

	//  emplace
	for (auto i = 0; i < size; ++i)
		set.emplace({ i });

	for (auto i = 0; i < set.size(); ++i)
		if (set.at({ i }) != i)
			FAIL() << "Invalid dictionary value [ 1 ]";


	for (auto it = set.cbegin() + 1; it != set.cend(); ++it)
		if (it->x > it->x)
			FAIL() << "set is unsorted [ 0 ]";

	// erase
	auto arr = agl::vector<bool>{};
	arr.resize(size);
	for (auto i = 0; i < size; ++i)
		arr[i] = true;

	/*
	auto index = agl::simple_rand(0, 10000);
	while (!dict.empty())
	{
		while (!arr[index])
			index = agl::simple_rand(0, 9999);

		arr[index] = false;

		dict.erase(dict.cbegin() + index);

		if (dict.find(index) != dict.end())
			FAIL() << "Invalid dictionary erase algorithm [ 0 ]";
	}
	*/
}