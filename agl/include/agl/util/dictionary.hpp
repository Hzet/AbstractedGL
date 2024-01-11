#pragma once
#include <algorithm>
#include "agl/util/pair.hpp"
#include "agl/util/vector.hpp"

namespace agl
{
template <typename TKey, typename TValue, typename TAlloc>
class dictionary
{
public:
	using data_type = pair<TKey, TValue>;
	using vector_type = vector<data_type, TAlloc>;
	using iterator = vector_type::iterator;
	using const_iterator = vector_type::const_iterator;
	using reverse_iterator = vector_type::reverse_iterator;
	using reverse_const_iterator = vector_type::reverse_const_iterator;

	dictionary(TAlloc alloc = {}) noexcept
		: m_data{ alloc }
	{
	}

	iterator begin() noexcept
	{
		return m_data.begin();
	}
	iterator rbegin() noexcept
	{
		return m_data.rbegin();
	}

	const_iterator cbegin() noexcept
	{
		return m_data.cbegin();
	}

	const_iterator crbegin() noexcept
	{
		return m_data.crbegin();
	}

	iterator end() noexcept
	{
		return m_data.end();
	}

	iterator rend() noexcept
	{
		return m_data.rend();
	}

	const_iterator cend() noexcept
	{
		return m_data.cend();
	}

	const_iterator crend() noexcept
	{
		return m_data.crend();
	}

	void clear() noexcept
	{
		m_data.clear();
	}

	iterator find(TKey key) noexcept
	{
		auto const comparator = [](data_type const& data, TKey const& key) { return data.first < key; };
		return std::lower_bound(m_data.begin(), m_data.end(), key, comparator);
	}

	const_iterator find(TKey key) const noexcept
	{
		auto const comparator = [](data_type const& data, TKey const& key) { return data.first < key; };
		return std::lower_bound(m_data.cbegin(), m_data.cend(), key, comparator);
	}

	iterator erase(const_iterator pos) noexcept
	{
		return m_data.erase(pos);
	}

	iterator erase(const_iterator first, const_iterator last) noexcept
	{
		return m_data.erase(first, last);
	}

	iterator emplace(TKey key, TValue value = {}) noexcept
	{
		auto const it = find(key);
		return m_data.insert(it, value);
	}
	bool empty() const noexcept
	{
		return m_data.empty();
	}

	TValue& at(TKey key) const noexcept
	{
		return *find(key)->second;
	}

	TValue const& at(TKey key) const noexcept
	{
		return *find(key)->second;
	}

	TValue& operator[](TKey key) noexcept
	{
		auto found = find(key);
		if (found == end())
			return emplace(key);

		return found->second;
	}

	TValue const& operator[](TKey key) const noexcept
	{
		auto found = find(key);
		if (found == cend())
			return emplace(key);

		return found->second;
	}

	std::uint64_t size() const noexcept
	{
		return m_data.size();
	}

	TAlloc get_allocator() const noexcept
	{
		return m_data.get_allocator();
	}

private:
	vector_type m_data;
};
}