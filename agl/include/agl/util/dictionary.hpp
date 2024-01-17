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
	using key_type = TKey;
	using mapped_type = TValue;
	using allocator_type = typename TAlloc::template rebind<pair<TKey, TValue>>;
	using value_type = typename type_traits<pair<TKey, TValue>>::value_type;
	using pointer = typename type_traits<pair<TKey, TValue>>::pointer;
	using const_pointer = typename type_traits<pair<TKey, TValue>>::const_pointer;
	using reference = typename type_traits<pair<TKey, TValue>>::reference;
	using const_reference = typename type_traits<pair<TKey, TValue>>::const_reference;
	using size_type = std::uint64_t;
	using difference_type = std::uint64_t;
	using vector_type = vector<typename value_type, typename allocator_type>;

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
		if (empty())
			return end();

		auto const comparator = [](value_type const& data, key_type const& key) { return data.first < key; };
		return std::lower_bound(m_data.begin(), m_data.end(), key, comparator);
	}
	const_iterator find(key_type key) const noexcept
	{
		if (empty())
			return cend();

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
		
		AGL_ASSERT(it == end(), "Key already stored");

		return m_data.insert(it, std::move(value_type{ std::move(key), std::move(value) }));
	}
	bool empty() const noexcept
	{
		return m_data.empty();
	}
	mapped_type& at(key_type key) noexcept
	{
		auto found = find(key);

		AGL_ASSERT(found != end(), "Index out of bounds");

		return found->second;
	}
	mapped_type const& at(key_type key) const noexcept
	{
		auto found = find(key);

		AGL_ASSERT(found != cend(), "Index out of bounds");

		return found->second;
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