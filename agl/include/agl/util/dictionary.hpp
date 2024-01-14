#pragma once
#include <algorithm>
#include "agl/util/pair.hpp"
#include "agl/util/memory/allocator.hpp"
#include "agl/util/vector.hpp"

namespace agl
{
template <typename TKey, typename TValue, typename TAlloc = mem::allocator<pair<TKey, TValue>>>
class dictionary
{
public:
	using allocator_type = TAlloc;
	using key_type = TKey;
	using mapped_type = TValue;
	using value_type = pair<key_type, mapped_type>;
	using pointer = typename allocator_type::pointer;
	using const_pointer = typename allocator_type::const_pointer;
	using reference = typename allocator_type::reference;
	using const_reference = typename allocator_type::const_reference;
	using size_type = typename allocator_type::size_type;
	using difference_type = typename allocator_type::difference_type;
	using vector_type = typename vector<value_type, allocator_type>;
	using iterator = typename vector_type::iterator;
	using const_iterator = typename vector_type::const_iterator;
	using reverse_iterator = typename vector_type::reverse_iterator;
	using reverse_const_iterator = typename vector_type::reverse_const_iterator;

	dictionary(allocator_type alloc = {}) noexcept
		: m_data{ alloc }
	{
	}

	iterator begin() const noexcept
	{
		return m_data.begin();
	}
	iterator rbegin() const noexcept
	{
		return m_data.rbegin();
	}

	const_iterator cbegin() const noexcept
	{
		return m_data.cbegin();
	}

	const_iterator crbegin() const noexcept
	{
		return m_data.crbegin();
	}

	iterator end() const noexcept
	{
		return m_data.end();
	}

	iterator rend() const noexcept
	{
		return m_data.rend();
	}

	const_iterator cend() const noexcept
	{
		return m_data.cend();
	}

	const_iterator crend() const noexcept
	{
		return m_data.crend();
	}

	void clear() noexcept
	{
		m_data.clear();
	}

	iterator find(key_type key) noexcept
	{
		auto const comparator = [](value_type const& data, key_type const& key) { return data.first < key; };
		return std::lower_bound(m_data.begin(), m_data.end(), key, comparator);
	}

	const_iterator find(key_type key) const noexcept
	{
		auto const comparator = [](value_type const& data, key_type const& key) { return data.first < key; };
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

	iterator emplace(key_type key, mapped_type value = {}) noexcept
	{
		auto const it = find(key);
		return m_data.insert(it, std::move(value_type{ std::move(key), std::move(value) }));
	}
	bool empty() const noexcept
	{
		return m_data.empty();
	}

	mapped_type& at(key_type key) noexcept
	{
		return find(key)->second;
	}

	mapped_type const& at(key_type key) const noexcept
	{
		return find(key)->second;
	}

	mapped_type& operator[](key_type key) noexcept
	{
		auto found = find(key);
		if (found == end())
			return emplace(key)->second;

		return found->second;
	}

	mapped_type const& operator[](key_type key) const noexcept
	{
		auto found = find(key);
		if (found == cend())
			return emplace(key);

		return found->second;
	}

	size_type size() const noexcept
	{
		return m_data.size();
	}

	allocator_type get_allocator() const noexcept
	{
		return m_data.get_allocator();
	}

private:
	vector_type m_data;
};
}