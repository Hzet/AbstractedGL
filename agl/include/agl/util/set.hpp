#pragma once
#include "agl/util/vector.hpp"

namespace agl
{
/**
 * @brief
 * Shares properties with 'std::set'. Uses 'std::lower_bound' as search algorithm.
 * @tparam T 
 * @tparam TComp 
 * @tparam TAlloc 
 */
template <typename T, typename TComp = std::less<T>, typename TAlloc = mem::allocator<T>>
class set
{
public:
	using comp_type = TComp;
	using allocator_type = typename TAlloc::template rebind<T>;
	using value_type = typename type_traits<T>::value_type;
	using pointer = typename type_traits<T>::pointer;
	using const_pointer = typename type_traits<T>::const_pointer;
	using reference = typename type_traits<T>::reference;
	using const_reference = typename type_traits<T>::const_reference;
	using size_type = typename type_traits<T>::size_type;
	using difference_type = typename type_traits<T>::difference_type;
	using data_type = vector<T, allocator_type>;

	using iterator = typename data_type::iterator;
	using const_iterator = typename data_type::const_iterator;
	using reverse_iterator = typename data_type::reverse_iterator;
	using reverse_const_iterator = typename data_type::reverse_const_iterator;

public:
	set() noexcept 
	{
	}
	explicit set(allocator_type const& allocator) noexcept
		: m_data{ allocator }
	{
	}
	explicit set(comp_type const& comp, allocator_type const& allocator) noexcept
		: m_comp{ comp }
		, m_data{ allocator }
	{
	}
	set(set&& other) noexcept
		: m_comp{ std::move(other.m_comp) }
		, m_data{ std::move(other.m_data) }
	{
	}
	set(set const& other) noexcept
		: m_comp{ other.m_comp }
		, m_data{ other.m_data }
	{
	}
	set& operator=(set&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_comp = std::move(other.m_comp);
		m_data = std::move(other.m_data);
		return *this;
	}
	set& operator=(set const& other) noexcept
	{
		if (this == &other)
			return *this;

		m_comp = other.m_comp;
		m_data = other.m_data;
		return *this;
	}
	~set() noexcept
	{
	}
	iterator begin() noexcept
	{
		return m_data.begin();
	}
	const_iterator begin() const noexcept
	{
		return m_data.cbegin();
	}
	iterator rbegin() noexcept
	{
		return m_data.rbegin();
	}	
	const_iterator rbegin() const noexcept
	{
		return m_data.crbegin();
	}
	const_iterator cbegin() const noexcept
	{
		return m_data.cbegin();
	}
	const_iterator crbegin() const noexcept
	{
		return m_data.crbegin();
	}
	iterator end() noexcept
	{
		return m_data.end();
	}
	const_iterator end() const noexcept
	{
		return m_data.end();
	}
	iterator rend() noexcept
	{
		return m_data.rend();
	}
	const_iterator rend() const noexcept
	{
		return m_data.crend();
	}
	const_iterator cend() const noexcept
	{
		return m_data.cend();
	}
	const_iterator crend() const noexcept
	{
		return m_data.crend();
	}
	reference front() noexcept
	{
		return m_data.front();
	}
	const_reference front() const noexcept
	{
		return m_data.front();
	}
	reference back() noexcept
	{
		return m_data.back();
	}
	const_reference back() const noexcept
	{
		return m_data.const();
	}
	void clear() noexcept
	{
		m_comp = {};
		m_data.clear();
	}
	void reserve(size_type n) noexcept
	{
		m_data.reserve(n);
	}
	void resize(size_type n) noexcept
	{
		m_data.resize(n);
	}
	template <typename U>
	iterator find(U const& value) noexcept
	{
		auto const found = std::lower_bound(m_data.begin(), m_data.end(), value, key_comp());

		if (found == end() || !equal(*found, value))
			return end();
		return found;
	}
	template <typename U>
	const_iterator find(U const& value) const noexcept
	{
		auto const found = std::lower_bound(m_data.cbegin(), m_data.cend(), value, key_comp());

		if (found == cend() || !equal(*found, value))
			return cend();
		return found;
	}
	comp_type key_comp() const noexcept
	{
		return m_comp;
	}
	reference at(value_type value) noexcept
	{
		auto const found = std::lower_bound(m_data.begin(), m_data.end(), value, key_comp());

		AGL_ASSERT(found != m_data.end() && equal(*found, value), "Index out of bounds");

		return *found;
	}
	const_reference at(value_type value) const noexcept
	{
		auto const found = std::lower_bound(m_data.begin(), m_data.end(), value, key_comp());

		AGL_ASSERT(found != m_data.cend() && equal(*found, value), "Index out of bounds");

		return *found;
	}
	iterator emplace(value_type&& value) noexcept
	{
		auto const it = std::lower_bound(m_data.cbegin(), m_data.cend(), value, key_comp());
		return m_data.insert(it, std::move(value));
	}
	iterator emplace(value_type const& value) noexcept
	{
		auto const it = std::lower_bound(m_data.cbegin(), m_data.cend(), value, key_comp());
		return m_data.insert(it, value);
	}
	bool empty() const noexcept
	{
		return m_data.empty();
	}
	iterator erase(const_iterator pos) noexcept
	{
		return m_data.erase(pos);
	}
	iterator erase(const_iterator first, const_iterator last) noexcept
	{
		return m_data.erase(first, last);
	}
	size_type size() const noexcept
	{
		return m_data.size();
	}

private:
	template <typename U, typename W>
	bool equal(U const& lhs, W const& rhs) const noexcept
	{
		return !m_comp(lhs, rhs) && !m_comp(rhs, lhs);
	}

private:
	comp_type m_comp;
	data_type m_data;
};
}