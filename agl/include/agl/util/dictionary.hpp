#pragma once
#include <algorithm>
#include "agl/util/memory/allocator.hpp"
#include "agl/util/vector.hpp"

namespace agl
{
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

		bool operator()(value_type const& lhs, key_type const& rhs) const noexcept
		{
			return comp(lhs.first, rhs);
		}
		bool operator()(key_type const& lhs, value_type const& rhs) const noexcept
		{
			return comp(lhs, rhs.first);
		}
	};

public:
	dictionary() noexcept
	{
	}
	explicit dictionary(allocator_type allocator) noexcept
		: m_data{ allocator }
	{
	}
	explicit dictionary(comp_type const& comp, allocator_type const& allocator) noexcept
		: m_comp{ .comp = comp }
		, m_data{ allocator }
	{
	}
	dictionary(dictionary&& other) noexcept
		: m_comp{ std::move(other.m_comp) }
		, m_data{ std::move(other.m_data) }
	{
	}
	dictionary(dictionary const& other) noexcept
		: m_comp{ other.m_comp }
		, m_data{ other.m_data }
	{
	}
	dictionary& operator=(dictionary&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_comp = std::move(other.m_comp);
		m_data = std::move(other.m_data);
		return *this;
	}
	dictionary& operator=(dictionary const& other) noexcept
	{
		if (this == &other)
			return *this;

		m_comp = other.m_comp;
		m_data = other.m_data;
		return *this;
	}
	~dictionary() noexcept
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
	comp_type key_comp() const noexcept
	{
		return m_comp.comp;
	}	
	key_value_comp_type key_value_comp() const noexcept
	{
		return m_comp;
	}
	iterator find(key_type key) noexcept
	{
		auto found = lower_bound(key);

		if (found == m_data.end() || !equal(found->first, key))
			return end();
		return found;
	}
	const_iterator find(key_type key) const noexcept
	{
		auto const found = lower_bound(key);

		if (found == m_data.cend() || !equal(found->first, key))
			return cend();
		return found;
	}
	iterator erase(const_iterator pos) noexcept
	{
		return m_data.erase(pos);
	}
	iterator erase(const_iterator first, const_iterator last) noexcept
	{
		return m_data.erase(first, last);
	}
	template <typename... TArgs>
	iterator emplace(TArgs&&... args) noexcept
	{
		auto pair = value_type{};
		if constexpr (sizeof... (TArgs) == 1)
			pair = std::make_pair(std::forward<TArgs>(args)..., mapped_type{});
		else 
			pair = std::make_pair(std::forward<TArgs>(args)...);

		auto const it = lower_bound(pair.first);

		AGL_ASSERT(find(pair.first) == end(), "Key already stored");

		return m_data.insert(it, std::move(pair));
	}
	bool empty() const noexcept
	{
		return m_data.empty();
	}
	mapped_type& at(key_type key) noexcept
	{
		auto const found = find(key);

		AGL_ASSERT(found != end(), "Index out of bounds");

		return found->second;
	}
	mapped_type const& at(key_type key) const noexcept
	{
		auto const found = find(key);

		AGL_ASSERT(found != cend(), "Index out of bounds");

		return found->second;
	}
	mapped_type& operator[](key_type key) noexcept
	{
		auto const found = lower_bound(key);

		if (found == end() || !equal(found->first, key))
			return m_data.insert(found, value_type{ key, {} })->second;
		return found->second;
	}
	mapped_type const& operator[](key_type key) const noexcept
	{
		auto const found = lower_bound(key);

		if (found == cend() || !equal(found->first, key))
			return m_data.insert(found, value_type{ key, {} })->second;
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
	iterator lower_bound(key_type key) noexcept
	{
		return std::lower_bound(m_data.begin(), m_data.end(), key, key_value_comp());
	}
	const_iterator lower_bound(key_type key) const noexcept
	{
		return std::lower_bound(m_data.cbegin(), m_data.cend(), key, key_value_comp());
	}
	bool equal(TKey const& lhs, TKey const& rhs) const noexcept
	{
		return !m_comp.comp(lhs, rhs) && !m_comp.comp(rhs, lhs);
	}

private:
	key_value_comp_type m_comp;
	vector_type m_data;
};
}