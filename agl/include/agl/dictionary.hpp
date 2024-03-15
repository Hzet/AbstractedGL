#pragma once
#include <algorithm>
#include "agl/memory/allocator.hpp"
#include "agl/vector.hpp"

namespace agl
{
/**
 * @brief 
 * Shares properties with 'std::map'. Search algorithm is 'std::lower_bound'.
 * @tparam TKey 
 * @tparam TValue 
 * @tparam TComp 
 * @tparam TAlloc 
 */
template <typename TKey, typename TValue, typename TComp = std::less<TKey>, typename TAlloc = mem::allocator<std::pair<TKey, TValue>>>
class dictionary
{
public:
	using value_type = typename type_traits<std::pair<TKey, TValue>>::value_type;
	using pointer = typename type_traits<std::pair<TKey, TValue>>::pointer;
	using const_pointer = typename type_traits<std::pair<TKey, TValue>>::const_pointer;
	using reference = typename type_traits<std::pair<TKey, TValue>>::reference;
	using const_reference = typename type_traits<std::pair<TKey, TValue>>::const_reference;
	using size_type = std::uint64_t;
	using difference_type = std::uint64_t;

	using comp_type = TComp;
	using key_type = TKey;
	using mapped_type = TValue;
	using allocator_type = typename TAlloc::template rebind<std::pair<TKey, TValue>>;
	using vector_type = vector<typename value_type, typename allocator_type>;

	using iterator = typename vector_type::iterator;
	using const_iterator = typename vector_type::const_iterator;
	using reverse_iterator = typename vector_type::reverse_iterator;
	using reverse_const_iterator = typename vector_type::reverse_const_iterator;

public:
	struct key_value_comp_type
	{
		comp_type comp;

		bool operator()(value_type const& lhs, key_type const& rhs) const
		{
			return comp(lhs.first, rhs);
		}
		bool operator()(key_type const& lhs, value_type const& rhs) const
		{
			return comp(lhs, rhs.first);
		}
	};

public:
	dictionary()
	{
	}
	explicit dictionary(allocator_type allocator)
		: m_data{ allocator }
	{
	}
	explicit dictionary(comp_type const& comp, allocator_type const& allocator)
		: m_comp{ .comp = comp }
		, m_data{ allocator }
	{
	}
	dictionary(dictionary&& other)
		: m_comp{ std::move(other.m_comp) }
		, m_data{ std::move(other.m_data) }
	{
	}
	dictionary(dictionary const& other)
		: m_comp{ other.m_comp }
		, m_data{ other.m_data }
	{
	}
	dictionary& operator=(dictionary&& other)
	{
		if (this == &other)
			return *this;

		m_comp = std::move(other.m_comp);
		m_data = std::move(other.m_data);
		return *this;
	}
	dictionary& operator=(dictionary const& other)
	{
		if (this == &other)
			return *this;

		m_comp = other.m_comp;
		m_data = other.m_data;
		return *this;
	}
	~dictionary()
	{
	}
	reference front()
	{
		return m_data.front();
	}
	const_reference front() const
	{
		return m_data.front();
	}
	reference back()
	{
		return m_data.back();
	}
	const_reference back() const
	{
		return m_data.back();
	}
	iterator begin()
	{
		return m_data.begin();
	}
	const_iterator begin() const
	{
		return m_data.begin();
	}
	iterator rbegin()
	{
		return m_data.rbegin();
	}
	const_iterator rbegin() const
	{
		return m_data.rbegin();
	}
	const_iterator cbegin() const
	{
		return m_data.cbegin();
	}
	const_iterator crbegin() const
	{
		return m_data.crbegin();
	}
	iterator end()
	{
		return m_data.end();
	}
	const_iterator end() const
	{
		return m_data.cend();
	}
	iterator rend()
	{
		return m_data.rend();
	}
	const_iterator rend() const
	{
		return m_data.crend();
	}
	const_iterator cend() const
	{
		return m_data.cend();
	}
	const_iterator crend() const
	{
		return m_data.crend();
	}
	void clear()
	{
		m_data.clear();
	}
	comp_type key_comp() const
	{
		return m_comp.comp;
	}	
	key_value_comp_type key_value_comp() const
	{
		return m_comp;
	}
	iterator find(key_type key)
	{
		auto found = lower_bound(key);

		if (found == m_data.end() || !equal(found->first, key))
			return end();
		return found;
	}
	const_iterator find(key_type key) const
	{
		auto const found = lower_bound(key);

		if (found == m_data.cend() || !equal(found->first, key))
			return cend();
		return found;
	}
	iterator erase(const_iterator pos)
	{
		return m_data.erase(pos);
	}
	iterator erase(const_iterator first, const_iterator last)
	{
		return m_data.erase(first, last);
	}
	iterator emplace(value_type&& pair)
	{
		AGL_ASSERT(find(pair.first) == end(), "Key already stored");

		auto const it = lower_bound(pair.first);
		return m_data.insert(it, std::forward<value_type>(pair));
	}
	bool empty() const
	{
		return m_data.empty();
	}
	mapped_type& at(key_type key)
	{
		auto found = find(key);

		AGL_ASSERT(found != end(), "Index out of bounds");

		return found->second;
	}
	mapped_type const& at(key_type key) const
	{
		auto const found = find(key);

		AGL_ASSERT(found != cend(), "Index out of bounds");

		return found->second;
	}
	mapped_type& operator[](key_type key)
	{
		auto found = lower_bound(key);

		if (found == end() || !equal(found->first, key))
			return m_data.insert(found, std::make_pair(key, mapped_type{}))->second;
		return found->second;
	}
	mapped_type const& operator[](key_type key) const
	{
		auto const found = lower_bound(key);

		AGL_ASSERT(found != cend() && equal(found->first, key), "Index out of bounds");

		return found->second;
	}
	size_type size() const
	{
		return m_data.size();
	}
	allocator_type get_allocator() const
	{
		return m_data.get_allocator();
	}

private:
	iterator lower_bound(key_type key)
	{
		return std::lower_bound(m_data.begin(), m_data.end(), key, key_value_comp());
	}
	const_iterator lower_bound(key_type key) const
	{
		return std::lower_bound(m_data.cbegin(), m_data.cend(), key, key_value_comp());
	}
	bool equal(TKey const& lhs, TKey const& rhs) const
	{
		return !m_comp.comp(lhs, rhs) && !m_comp.comp(rhs, lhs);
	}

private:
	key_value_comp_type m_comp;
	vector_type m_data;
};
}