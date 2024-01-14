#pragma once
#include "agl/util/vector.hpp"

namespace agl
{
namespace impl
{
template <typename T, typename TAlloc>
class block
{
	using allocator_type = typename TAlloc;
	using value_type = typename TAlloc::value_type;
	using pointer = typename TAlloc::pointer;
	using const_pointer = typename TAlloc::const_pointer;
	using reference = typename TAlloc::reference;
	using const_reference = typename TAlloc::const_reference;
	using size_type = typename TAlloc::size_type;
	using difference_type = typename TAlloc::difference_type;

public:
	block(std::uint64_t block_size, allocator_type const& allocator) noexcept
		: m_data{ allocator }
		, m_free_indexes{ allocator }
		, m_indexes{ allocator }
		, m_size{ 0 }
	{
	}
	void clear() noexcept
	{
		m_data.clear();
		m_free_indexes.clear();
		m_size = 0;
	}
	bool empty() const noexcept
	{
		return m_size == 0;
	}
	void erase(difference_type index) noexcept
	{
		--m_size;
		m_data.erase(m_data.cbegin() + m_indexes[index]);
		m_free_indexes.push_back(m_indexes[index]);

		m_indexes.resize(size());
	}
	void erase(difference_type first, difference_type last) noexcept
	{
		auto const erase_size = last - first;
		m_size -= erase_size;
		m_data.erase(m_data.cbegin() + m_indexes[first], m_data.cbegin() + m_indexes[last]);

		for (auto i = m_indexes[first]; i < m_indexes[last]; ++i)
			m_free_indexes.push_back(m_indexes[i]);

		m_indexes.resize(size());
	}
	void reserve(std::uint64_t n) noexcept
	{
		m_data.resize(n);
		m_free_indexes.resize(n);

		for (auto i = 0; i < m_free_indexes.size(); ++i)
			m_free_indexes[i] = i;

		m_block_size = n;
	}
	difference_type push_back(value_type&& value) noexcept
	{
		if (empty())
			reserve(block_size());

		auto const index = m_free_indexes.back();
		m_free_indexes.pop_back();
		m_data[index] = std::move(value);
		m_indexes.push_back(index);
		++m_size;

		return index;
	}
	difference_type push_back(value_type const& value) noexcept
	{
		if (empty())
			reserve(block_size());

		auto const index = m_free_indexes.back();
		m_free_indexes.pop_back();
		m_data[index] = value;
		m_indexes.push_back(index);
		++m_size;

		return index;
	}
	size_type size() const noexcept
	{
		return m_size;
	}
	size_type block_size() const noexcept
	{
		return m_block_size;
	}
	reference operator[](size_type index) noexcept
	{
		return m_data[m_indexes[index]];
	}
	const_reference operator[](size_type index) const noexcept
	{
		return m_data[m_indexes[index]];
	}

private:
	vector<T, TAlloc> m_data;
	vector<std::uint16_t, TAlloc> m_free_indexes;
	vector<std::uint16_t, TAlloc> m_indexes;
	size_type m_size;
	size_type m_block_size;
};

template <typename T, typename TDeque>
class deque_iterator;
template <typename T, typename TDeque>
class deque_const_iterator;
template <typename T, typename TDeque>
class reverse_deque_iterator;
template <typename T, typename TDeque>
class reverse_deque_const_iterator;
}
template <typename T, typename TAlloc = agl::mem::allocator<T>>
class deque
{
public:
	using self = typename deque<T, TAlloc>;
	using allocator_type = typename TAlloc;
	using value_type = typename TAlloc::value_type;
	using pointer = typename TAlloc::pointer;
	using const_pointer = typename TAlloc::const_pointer;
	using reference = typename TAlloc::reference;
	using const_reference = typename TAlloc::const_reference;
	using size_type = typename TAlloc::size_type;
	using difference_type = typename TAlloc::difference_type;
	using iterator = typename impl::deque_iterator<T, self>;
	using const_iterator = typename impl::deque_const_iterator<T, self>;
	using reverse_iterator = typename impl::reverse_deque_iterator<T, self>;
	using reverse_const_iterator = typename impl::reverse_deque_const_iterator<T, self>;

public:
	deque(allocator_type const& allocator) noexcept
		: m_blocks{ allocator }
		, m_size{ 0 }
	{
	}

	void clear() noexcept
	{
		for (auto& block : m_blocks)
			block.clear();
	}
	void erase() noexcept
	{
	}
	void push_back(value_type&& value) noexcept
	{

	}
	void push_back(value_type const& value) noexcept;
	void push_front(value_type&& value) noexcept;
	void push_front(value_type const& value) noexcept;

	iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end() const noexcept;
	const_iterator cend() const noexcept;

	reverse_iterator rbegin() const noexcept;
	reverse_const_iterator crbegin() const noexcept;
	reverse_iterator rend() const noexcept;
	reverse_const_iterator crend() const noexcept;

	size_type size() const noexcept;
	size_type block_size() const noexcept;
	
	reference at(size_type index) noexcept;
	const_reference at(size_type index) const noexcept;
	reference operator[](size_type index) noexcept;
	const_reference operator[](size_type index) const noexcept;

private:
	/// <summary>
	/// Finds a block that contains item residing under index 'value_index'.
	/// </summary>
	/// <param name="value_index"></param>
	/// <returns></returns>
	impl::block& find_block(size_type value_index) const noexcept
	{
		
	}

private:
	size_type m_size;
	vector<impl::block, TAlloc> m_blocks;
};
}