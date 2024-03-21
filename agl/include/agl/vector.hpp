#pragma once
#include <cstdint>
#include <type_traits>
#include "agl/core/debug.hpp"
#include "agl/util/iterator.hpp"
#include "agl/memory/allocator.hpp"

namespace agl
{
template <typename T, typename TTraits>
class vector_iterator;

template <typename T, typename TTraits>
class vector_reverse_iterator;

namespace impl
{
template <typename T>
struct iterator_traits
{
	using value_type = T;
	using pointer = T*;
	using const_pointer = T const*;
	using reference = T&;
	using const_reference = T const&;
	using difference_type = std::uint64_t;
};
template <typename T>
struct const_iterator_traits
{
	using value_type = T;
	using pointer = T const*;
	using const_pointer = T const*;
	using reference = T const&;
	using const_reference = T const&;
	using difference_type = std::uint64_t;
};
}

/**
 * @brief 
 * Shares properties with 'std::vector'.
 * @tparam T 
 * @tparam TAlloc 
 */
template <typename T, typename TAlloc = mem::allocator<T>>
class vector
{
public:
	using allocator_type = typename TAlloc::template rebind<T>;
	using value_type = typename type_traits<T>::value_type;
	using pointer = typename type_traits<T>::pointer;
	using const_pointer = typename type_traits<T>::const_pointer;
	using reference = typename type_traits<T>::reference;
	using const_reference = typename type_traits<T>::const_reference;
	using size_type = typename type_traits<T>::size_type;
	using difference_type = typename type_traits<T>::difference_type;

	using iterator = vector_iterator<T, impl::iterator_traits<T>>;
	using const_iterator = vector_iterator<T, impl::const_iterator_traits<T>>;
	using reverse_iterator = vector_reverse_iterator<T, impl::iterator_traits<T>>;
	using reverse_const_iterator = vector_reverse_iterator<T, impl::const_iterator_traits<T>>;

	vector()
		: m_allocator{ }
		, m_capacity{ 0 }
		, m_memory{ nullptr }
		, m_size{ 0 }
	{
	}
	explicit vector(allocator_type const& alloc)
		: m_allocator{ alloc }
		, m_capacity{ 0 }
		, m_memory{ nullptr }
		, m_size{ 0 }
	{
	}
	template <typename TInputIt, typename TEnable = impl::is_iterator<TInputIt>>
	vector(TInputIt first, TInputIt last)
	{
		assign(first, last);
	}
	vector(std::initializer_list<T> list)
	{
		assign(list.begin(), list.end());
	}
	vector(vector&& other)
		: m_allocator{ std::move(other.m_allocator) }
		, m_capacity{ other.m_capacity }
		, m_memory{ other.m_memory }
		, m_size{ other.m_size }
	{
		other.m_capacity = 0;
		other.m_size = 0;
		other.m_memory = nullptr;
	}
	vector(vector const& other)
		: vector{ other.m_allocator }
	{
		if (other.m_memory == nullptr)
			return;

		reserve(other.size());
		for (auto const& v : other)
			push_back(v);
	}
	vector& operator=(vector&& other)
	{
		if (this == &other)
			return *this;

		clear();

		m_allocator = std::move(other.m_allocator);
		m_capacity = other.m_capacity;
		other.m_capacity = 0;
		m_memory = other.m_memory;
		other.m_memory = nullptr;
		m_size = other.m_size;
		other.m_size = 0;
		return *this;
	}
	vector& operator=(vector const& other)
	{
		if (this == &other)
			return *this;

		clear();
		m_allocator = other.m_allocator;

		reserve(other.m_capacity);
		for (auto const& v : other)
			this->push_back(v);

		return *this;
	}
	~vector()
	{
		clear();
	}
	reference at(size_type index)
	{
		AGL_ASSERT(index < size(), "Index out of bounds");
		AGL_ASSERT(m_memory != nullptr, "Index out of bounds");

		return *(m_memory + index);
	}
	const_reference at() const
	{
		AGL_ASSERT(index < size(), "Index out of bounds");
		AGL_ASSERT(m_memory != nullptr, "Index out of bounds");

		return *(m_memory + index);
	}
	void assign(size_type count, const_reference value)
	{
		if (count != size())
		{
			clear();
			resize(count);
		}
		
		for (auto i = 0; i < count; ++i)
			make_copy(m_memory + i, value);
	}
	template <typename TInputIt, typename TEnable = impl::is_iterator_t<TInputIt>>
	void assign(TInputIt first, TInputIt last)
	{
		auto const count = last - first;
		if (count != size())
		{
			clear();
			resize(count);
		}

		for (auto i = 0; i < count; ++i, ++first)
			make_copy(m_memory + i, *first);
	}
	iterator begin()
	{
		return make_iterator<iterator>(m_memory);
	}
	const_iterator begin() const
	{
		return cbegin();
	}
	const_iterator cbegin() const
	{
		return make_iterator<const_iterator>(m_memory);
	}
	iterator end()
	{
		return make_iterator<iterator>(m_memory + size());
	}
	const_iterator end() const
	{
		return cend();
	}
	const_iterator cend() const
	{
		return make_iterator<const_iterator>(m_memory + size());
	}
	reverse_iterator rbegin()
	{
		if (empty())
			return make_iterator<reverse_iterator>(nullptr);
		return make_iterator<reverse_iterator>(m_memory + size() - 1);
	}
	reverse_const_iterator rbegin() const
	{
		if (empty())
			return make_iterator<reverse_const_iterator>(nullptr);
		return make_iterator<reverse_const_iterator>(m_memory + size() - 1);
	}
	reverse_const_iterator crbegin() const
	{
		if (empty())
			return make_iterator<reverse_const_iterator>(nullptr);
		return make_iterator<reverse_const_iterator>(m_memory + size() - 1);
	}
	reverse_iterator rend()
	{
		if (empty())
			return make_iterator<reverse_iterator>(nullptr);
		return make_iterator<reverse_iterator>(m_memory - 1);
	}
	reverse_const_iterator rend() const
	{
		if (empty())
			return make_iterator<reverse_const_iterator>(nullptr);
		return make_iterator<reverse_const_iterator>(m_memory - 1);
	}
	reverse_const_iterator crend() const
	{
		if (empty())
			return make_iterator<reverse_const_iterator>(nullptr);
		return make_iterator<reverse_const_iterator>(m_memory - 1);
	}
	reference operator[](size_type index)
	{
		return *(m_memory + index);
	}
	const_reference operator[](size_type index) const
	{
		return *(m_memory + index);
	}
	reference front()
	{
		AGL_ASSERT(!empty(), "Index out of bounds");

		return *m_memory;
	}
	const_reference front() const
	{
		AGL_ASSERT(!empty(), "Index out of bounds");

		return *m_memory;
	}
	reference back()
	{
		AGL_ASSERT(!empty(), "Index out of bounds");

		return *(m_memory + m_size - 1);
	}
	const_reference back() const
	{
		AGL_ASSERT(!empty(), "Index out of bounds");

		return *(m_memory + m_size - 1);
	}
	pointer data()
	{
		return m_memory;
	}
	const_pointer data() const
	{
		return m_memory
	}
	bool empty() const
	{
		return m_size == 0;
	}
	allocator_type get_allocator() const
	{
		return m_allocator;
	}
	size_type size() const
	{
		return m_size;
	}
	void resize(size_type n)
	{
		if (n > capacity())
			reserve(n);
		m_size = n;
	}
	size_type capacity() const
	{
		return m_capacity;
	}
	void shrink_to_fit()
	{
		if (capacity() == size())
			return;
		realloc(size());
	}
	void clear()
	{
		if (m_memory == nullptr)
			return;

		for (auto i = difference_type{ 0 }; i < static_cast<difference_type>(capacity()); ++i)
			m_allocator.destruct(m_memory + i);
		m_allocator.deallocate(m_memory, capacity());
		m_memory = nullptr;
		m_size = 0;
		m_capacity = 0;
	}
	iterator insert(const_iterator pos, const_reference value)
	{
		AGL_ASSERT(cbegin() <= pos && pos <= cend(), "Index out of bounds");

		if (pos == cend() || empty())
		{
			push_back(value);
			return make_iterator<iterator>(m_memory + m_size - 1);
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		move_elements_right(begin() + index + 1, begin() + index);
		make_copy(m_memory + index, value);
		++m_size;
		return make_iterator<iterator>(m_memory + index);
	}
	iterator insert(const_iterator pos, value_type&& value)
	{
		AGL_ASSERT(cbegin() <= pos && pos <= cend(), "Index out of bounds");

		if (pos == cend() || empty())
		{
			push_back(std::move(value));
			return make_iterator<iterator>(m_memory + m_size - 1);
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		auto it = make_internal_iterator<iterator>(m_memory + index + 1);
		move_elements_right(it, it - 1);
		make_move(m_memory + index, std::forward<value_type&&>(value));
		++m_size;
		return make_iterator<iterator>(m_memory + index);
	}
	template <typename TInputIt, typename TEnable = impl::is_iterator<TInputIt>>
	iterator insert(const_iterator pos, TInputIt first, TInputIt last)
	{
		AGL_ASSERT(cbegin() <= pos && pos <= cend(), "Index out of bounds");
		AGL_ASSERT(cbegin() <= first && first <= cend(), "Index out of bounds");
		AGL_ASSERT(cbegin() <= last && last <= cend(), "Index out of bounds");

		auto const index = pos - begin();
		auto const insert_size = std::distance(last - first);
		if (pos == cend() || empty())
		{
			reserve(size() + insert_size);
			for (auto it = first; it != last; ++it)
				push_back(*it);
			return make_iterator<iterator>(m_memory + index);
		}
		reserve(size() + insert_size);
		move_elements_right(begin() + index + insert_size, begin() + index);
		
		for (auto i = difference_type{ 0 }; i < insert_size; ++i, ++first)
			make_move_or_copy(m_memory + index + i, std::forward<value_type&&>(*first));

		m_size += insert_size;
		return make_iterator<iterator>(m_memory + index);
	}
	template <typename... TArgs>
	iterator emplace(const_iterator pos, TArgs&&... args)
	{
		AGL_ASSERT(cbegin() <= pos && pos <= cend(), "Index out of bounds");

		if (pos == cend() || empty())
		{
			emplace_back(std::forward<TArgs>(args)...);
			return make_iterator<iterator>(m_memory + m_size - 1);
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		move_elements_right(begin() + index + 1, begin() + index);
		m_allocator.destruct(m_memory + index);
		m_allocator.construct(m_memory + index, std::forward<TArgs>(args)...);
		++m_size;
		return make_iterator<iterator>(m_memory + index);
	}
	template <typename... TArgs>
	iterator emplace_back(TArgs&&... args)
	{
		resize(size() + 1);
		m_allocator.destruct(m_memory + size() - 1);
		m_allocator.construct(m_memory + size() - 1, std::forward<TArgs>(args)...);
		return make_iterator<iterator>(m_memory + size() - 1);
	}
	template <typename... TArgs>
	iterator emplace_front(TArgs&&... args)
	{
		resize(size() + 1);
		move_elements_right(begin() + 1, begin());
		m_allocator.destruct(m_memory);
		m_allocator.construct(m_memory, std::forward<TArgs>(args)...);
		return begin();
	}
	iterator erase(const_iterator pos)
	{
		AGL_ASSERT(cbegin() <= pos && pos < cend(), "Iterator out of bounds");

		if (size() == 1)
		{
			clear();
			return end();
		}

		m_allocator.destruct(pos.m_ptr);
		m_allocator.construct(pos.m_ptr);
		auto it = make_iterator<iterator>(pos.m_ptr);
		move_elements_left(it, it + 1);
		--m_size;
		return it;
	}
	iterator erase(const_iterator first, const_iterator last)
	{
		AGL_ASSERT(cbegin() <= first && first < cend(), "Iterator out of bounds");
		AGL_ASSERT(cbegin() <= last && last < cend(), "Iterator out of bounds");

		if (first == begin() && last == cend())
		{
			clear();
			return end();
		}

		for (first; first != last; ++first)
		{
			m_allocator.destruct(&(*first));
			m_allocator.construct(&(*first));
		}

		move_elements_left(&(*first), &(*last))
			m_size -= erase_size;
		return make_iterator<iterator>(m_memory + offset);
	}
	void push_back(value_type&& value)
	{
 		resize(size() + 1);
		make_move(m_memory + size() - 1, std::forward<value_type&&>(value));
	}
	void push_back(const_reference value)
	{
		resize(size() + 1);
		make_copy(m_memory + size() - 1, value);
	}
	void pop_back()
	{
		AGL_ASSERT(!empty(), "erase on empty vector");

		m_allocator.destruct(m_memory + size() - 1);
		m_allocator.construct(m_memory + size() - 1);
		--m_size;
	}
	void pop_front()
	{
		erase(cbegin());
	}
	void reserve(size_type n)
	{
		if (n <= capacity())
			return;
		realloc(n);
	}
private:
	template <typename TIterator>
	TIterator make_iterator(T* ptr) const
	{
		return TIterator{ ptr, m_memory, m_memory + size() };
	}
	template <typename TIterator>
	TIterator make_internal_iterator(T* ptr) const // returns iterator checked up to capacity()
	{
		return TIterator{ ptr, m_memory, m_memory + capacity() };
	}
	iterator real_end() const
	{
		return iterator{ m_memory + capacity(), m_memory, m_memory + capacity() };
	}
	void make_copy(pointer dest, const_reference src)
	{
		AGL_ASSERT(m_memory <= dest && dest < m_memory + capacity(), "iterator out of range");
		
		if constexpr (std::is_copy_constructible_v<value_type>)
			m_allocator.construct(dest, src);
		else if constexpr (std::is_copy_assignable_v<value_type>)
			*dest = src;
		else
			static_assert(false, "invalid use of copy function - type is not copyable");
	}
	void make_move(pointer dest, value_type&& src)
	{
		AGL_ASSERT(m_memory <= dest && dest < m_memory + capacity(), "iterator out of range");

		if constexpr (std::is_move_constructible_v<value_type>)
			m_allocator.construct(dest, std::forward<value_type&&>(src));
		else if constexpr (std::is_move_assignable_v<value_type>)
			*dest = std::move(src);
		else
			static_assert(false, "invalid use of copy function - type is not movalbe");
	}
	void make_move_or_copy(pointer dest, value_type&& src)
	{
		make_move(dest, std::forward<value_type&&>(src));
	}
	void make_move_or_copy(pointer dest, const_reference src)
	{
		make_copy(dest, src);
	}
	/// <summary>
	/// Moves element 'from' and all the elements right to it to the position 'where'
	/// </summary>
	void move_elements_left(iterator where, iterator from)
	{
		AGL_ASSERT(where <= from, "Invalid data");
		AGL_ASSERT(begin() <= where && where < end(), "Index out of bounds");
		AGL_ASSERT(begin() <= from && from <= end(), "Index out of bounds");

		for (where, from; from != end(); ++where, ++from)
			make_move_or_copy(&(*where), std::move(*from));
	}
	void move_elements_right(iterator where, iterator from)
	{
		AGL_ASSERT(where > from, "Invalid data");
		AGL_ASSERT(begin() <= where && where < real_end(), "Index out of bounds");
		AGL_ASSERT(begin() <= from && from <= real_end(), "Index out of bounds");

		auto const offset = where - from;
		auto w = make_internal_iterator<reverse_iterator>(m_memory + size() + offset - 1);
		auto f = make_iterator<reverse_iterator>(m_memory + size() - 1);
		auto const end = make_iterator<reverse_iterator>(&(*from));

		for (w, f; w != end; ++w, ++f)
			make_move_or_copy(&(*w), std::move(*f));
	}
	void realloc(size_type n)
	{
		auto* tmp_buffer = m_allocator.allocate(n);

		// move current content to new buffer
		for (auto i = difference_type{ 0 }; i < static_cast<difference_type>(size()); ++i)
			m_allocator.construct(tmp_buffer + i, std::move(*(m_memory + i)));

		for (auto i = size(); i < n; ++i)
			m_allocator.construct(tmp_buffer + i);

		if (capacity() > 0)
		{
			for (auto i = difference_type{ 0 }; i < static_cast<difference_type>(capacity()); ++i)
				m_allocator.destruct(m_memory + i);

			m_allocator.deallocate(m_memory, capacity());
		}

		m_memory = tmp_buffer;
		m_capacity = n;
	}
private:
	allocator_type m_allocator;
	size_type m_capacity;
	pointer m_memory;
	size_type m_size;
};

namespace impl
{
template <typename T>
bool iterator_in_range(T* it, T* begin, T* end)
{
	return begin <= it && it <= end;
}
}

template <typename T, typename TTraits>
class vector_iterator
{
	template <typename U, typename UTraits>
	static constexpr auto is_const_iterator_v = std::is_same_v<UTraits, impl::const_iterator_traits<U>>;

public:
	using traits = TTraits;
	using value_type = typename traits::value_type;
	using data_pointer = T*;
	using pointer = typename traits::pointer;
	using const_pointer = typename traits::const_pointer;
	using reference = typename traits::reference;
	using const_reference = typename traits::const_reference;
	using difference_type = typename traits::difference_type;

	vector_iterator()
		: m_begin{ nullptr }
		, m_end{ nullptr }
		, m_ptr{ nullptr }
	{
	}
	vector_iterator(data_pointer ptr, data_pointer begin, data_pointer end)
		: m_begin{ begin }
		, m_end{ end }
		, m_ptr{ ptr }

	{
		AGL_ASSERT((m_ptr == nullptr && m_begin == nullptr && m_end == nullptr) || impl::iterator_in_range(m_ptr, m_begin, m_end), "invalid iterator");
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(is_const_iterator_v<T, UTraits> && ! is_const_iterator_v<T, TTraits>)>>//std::enable_if_t<!(std::is_same_v<UTraits, impl::const_iterator_traits<T>>&& std::is_same_v<TTraits, impl::iterator_traits<T>>)>>
	vector_iterator(vector_iterator<T, UTraits>&& other)
		: m_begin{ other.m_begin }
		, m_end{ other.m_end }
		, m_ptr{ other.m_ptr }
	{
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(is_const_iterator_v<T, UTraits> && !is_const_iterator_v<T, TTraits>)>>
	vector_iterator(vector_iterator<T, UTraits> const& other)
		: m_begin{ other.m_begin }
		, m_end{ other.m_end }
		, m_ptr{ other.m_ptr }
	{
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(is_const_iterator_v<T, UTraits> && !is_const_iterator_v<T, TTraits>)>>
	vector_iterator& operator=(vector_iterator<T, UTraits>&& other)
	{
		m_begin = other.m_begin;
		m_end = other.m_end;
		m_ptr = other.m_ptr;
		return *this;
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(is_const_iterator_v<T, UTraits> && !is_const_iterator_v<T, TTraits>)>>
	vector_iterator& operator=(vector_iterator<T, UTraits> const& other)
	{
		m_begin = other.m_begin;
		m_end = other.m_end; 
		m_ptr = other.m_ptr;
		return *this;
	}
	~vector_iterator()
	{
	}
	vector_iterator& operator++()
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid operation");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin, m_end - 1), "cannot increment past end");

		m_ptr += 1;
		return *this;
	}
	vector_iterator operator++(int) const
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid operation");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin, m_end - 1), "cannot increment past end");

		auto result = vector_iterator{ *this };
		return ++result;
	}
	vector_iterator operator+(difference_type offset) const
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr + offset, m_begin, m_end), "iterator out of range");

		return vector_iterator{ m_ptr + offset, m_begin, m_end };
	}
	vector_iterator& operator+=(difference_type offset)
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr + offset, m_begin, m_end), "iterator out of range");

		m_ptr += offset;
		return *this;
	}
	vector_iterator& operator--()
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin + 1, m_end), "cannot decrement past begin");

		m_ptr -= 1;
		return *this;
	}
	vector_iterator operator--(int) const
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin + 1, m_end), "cannot decrement past begin");

		auto result = vector_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_iterator rhs) const
	{
		AGL_ASSERT((m_ptr == nullptr && rhs.m_ptr == nullptr) || (m_ptr != nullptr && rhs.m_ptr != nullptr), "invalid operation");
		AGL_ASSERT(m_ptr == nullptr || impl::iterator_in_range(m_ptr, m_begin, m_end), "invalid iterator");
		AGL_ASSERT(rhs.m_ptr == nullptr || impl::iterator_in_range(rhs.m_ptr, m_begin, m_end), "invalid iterator");
		AGL_ASSERT(rhs.m_ptr == nullptr || *this >= rhs, "unsigned integer overflow");

		return m_ptr - rhs.m_ptr;
	}
	vector_iterator operator-(difference_type offset) const
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "invalid operation");
		AGL_ASSERT(impl::iterator_in_range(m_ptr - offset, m_begin, m_end), "iterator out of range");

		return vector_iterator{ m_ptr - offset, m_begin, m_end };
	}
	vector_iterator& operator-=(difference_type offset)
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "invalid operation");
		AGL_ASSERT(impl::iterator_in_range(m_ptr - offset, m_begin, m_end), "iterator out of range");

		m_ptr -= offset;
		return *this;
	}
	template <typename = std::enable_if_t<!is_const_iterator_v<T, TTraits>>>
	reference operator*()
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin, m_end), "cannot dereference outside-range iterator");

		return *m_ptr;
	}	
	const_reference operator*() const
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin, m_end), "cannot dereference outside-range iterator");

		return *m_ptr;
	}
	template <typename = std::enable_if_t<!is_const_iterator_v<T, TTraits>>>
	pointer operator->()
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin, m_end), "Cannot dereference outside-range iterator");

		return m_ptr;
	}
	const_pointer operator->() const
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin, m_end), "Cannot dereference outside-range iterator");

		return m_ptr;
	}

private:
	template <typename U, typename W>
	friend class vector;

	template <typename U, typename W>
	friend class vector_iterator;

	template <typename U, typename W>
	friend class vector_reverse_iterator;

	template <typename U, typename W>
	friend bool operator==(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator!=(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator<(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator<=(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator>(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator>=(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs);


private:
	data_pointer m_begin;
	data_pointer m_end;
	data_pointer m_ptr;
};

template <typename T, typename U>
bool operator==(vector_iterator<T, U> const& lhs, vector_iterator<T, U> const& rhs)
{
	return lhs.m_ptr == rhs.m_ptr;
}
template <typename T, typename U>
bool operator!=(vector_iterator<T, U> const& lhs, vector_iterator<T, U> const& rhs)
{
	return lhs.m_ptr != rhs.m_ptr;
}
template <typename T, typename U>
bool operator<(vector_iterator<T, U> const& lhs, vector_iterator<T, U> const& rhs)
{
	return lhs.m_ptr < rhs.m_ptr;
}
template <typename T, typename U>
bool operator<=(vector_iterator<T, U> const& lhs, vector_iterator<T, U> const& rhs)
{
	return lhs.m_ptr <= rhs.m_ptr;
}
template <typename T, typename U>
bool operator>(vector_iterator<T, U> const& lhs, vector_iterator<T, U> const& rhs)
{
	return lhs.m_ptr > rhs.m_ptr;
}
template <typename T, typename U>
bool operator>=(vector_iterator<T, U> const& lhs, vector_iterator<T, U> const& rhs)
{
	return lhs.m_ptr >= rhs.m_ptr;
}

template <typename T, typename TTraits>
class vector_reverse_iterator
{
public:
	using traits = TTraits;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using data_pointer = T*;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	vector_reverse_iterator()
		: m_begin{ nullptr }
		, m_end{ nullptr }
		, m_ptr{ nullptr }
	{
	}
	vector_reverse_iterator(data_pointer ptr, data_pointer begin, data_pointer end)
		: m_begin{ begin }
		, m_end{ end }
		, m_ptr{ ptr }

	{
		AGL_ASSERT((m_ptr == nullptr && m_begin == nullptr && m_end == nullptr) || impl::iterator_in_range(m_ptr, m_begin - 1, m_end - 1), "invalid iterator");
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(std::is_same_v<UTraits, impl::const_iterator_traits<T>>&& std::is_same_v<TTraits, impl::iterator_traits<T>>)>>
	explicit vector_reverse_iterator(vector_iterator<T, UTraits> const& other)
		: m_begin{ other.m_begin }
		, m_end{ other.m_end }
		, m_ptr{ other.m_ptr }
	{
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(std::is_same_v<UTraits, impl::const_iterator_traits<T>> && std::is_same_v<TTraits, impl::iterator_traits<T>>)>>
	vector_reverse_iterator(vector_reverse_iterator<T, UTraits>&& other)
		: m_begin{ other.m_begin }
		, m_end{ other.m_end }
		, m_ptr{ other.m_ptr }
	{
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(std::is_same_v<UTraits, impl::const_iterator_traits<T>>&& std::is_same_v<TTraits, impl::iterator_traits<T>>)>>
	vector_reverse_iterator(vector_reverse_iterator<T, UTraits> const& other)
		: m_begin{ other.m_begin }
		, m_end{ other.m_end }
		, m_ptr{ other.m_ptr }
	{
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(std::is_same_v<UTraits, impl::const_iterator_traits<T>>&& std::is_same_v<TTraits, impl::iterator_traits<T>>)>>
	vector_reverse_iterator& operator=(vector_reverse_iterator<T, UTraits>&& other)
	{
		m_ptr = other.m_ptr;
		m_begin = other.m_begin;
		m_end = other.m_end;
		return *this;
	}
	template <typename UTraits, typename TEnable = std::enable_if_t<!(std::is_same_v<UTraits, impl::const_iterator_traits<T>>&& std::is_same_v<TTraits, impl::iterator_traits<T>>)>>
	vector_reverse_iterator& operator=(vector_reverse_iterator<T, UTraits> const& other)
	{
		m_ptr = other.m_ptr;
		m_begin = other.m_begin;
		m_end = other.m_end;
		return *this;
	}
	~vector_reverse_iterator()
	{
	}
	vector_reverse_iterator& operator++()
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid operation");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin, m_end - 1), "cannot increment past rend");

		m_ptr -= 1;
		return *this;
	}
	vector_reverse_iterator operator++(int) const
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin, m_end - 1), "cannot increment past rend");

		auto result = vector_reverse_iterator{ *this };
		return ++result;
	}
	vector_reverse_iterator operator+(difference_type offset) const
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "invalid iterator");
		AGL_ASSERT(m_ptr == nullptr || impl::iterator_in_range(m_ptr - offset, m_begin - 1, m_end - 1), "iterator out of range");

		return vector_reverse_iterator{ m_ptr - offset, m_begin, m_end };
	}
	vector_reverse_iterator& operator+=(difference_type offset)
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "invalid iterator");
		AGL_ASSERT(m_ptr == nullptr || impl::iterator_in_range(m_ptr - offset, m_begin - 1, m_end - 1), "iterator out of range");

		m_ptr -= offset;
		return *this;
	}
	vector_reverse_iterator& operator--()
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(m_ptr != m_end - 1 && impl::iterator_in_range(m_ptr, m_begin - 1, m_end - 1), "cennot decrement past rbegin");

		m_ptr += 1;
		return *this;
	}
	vector_reverse_iterator operator--(int) const
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(m_ptr != m_end - 1 && impl::iterator_in_range(m_ptr, m_begin - 1, m_end - 1), "cennot decrement past rbegin");

		auto result = vector_reverse_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_reverse_iterator rhs) const
	{
		AGL_ASSERT((m_ptr == nullptr && rhs.m_ptr == nullptr) || (m_ptr != nullptr && rhs.m_ptr != nullptr), "invalid iterator");
		AGL_ASSERT(m_ptr == nullptr || impl::iterator_in_range(m_ptr, m_begin - 1, m_end - 1), "invalid iterator");
		AGL_ASSERT(m_ptr == nullptr || impl::iterator_in_range(rhs.m_ptr, m_begin - 1, m_end - 1), "Invalid iterator");
		AGL_ASSERT(m_ptr == nullptr || *this >= rhs, "unsigned integer overflow");

		return m_ptr - rhs.m_ptr;
	}
	vector_reverse_iterator operator-(difference_type offset) const
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "invalid operation");
		AGL_ASSERT(m_ptr == nullptr || impl::iterator_in_range(m_ptr + offset, m_begin - 1, m_end - 1), "iterator out of range");
		
		return vector_reverse_iterator{ m_ptr + offset, m_begin, m_end };
	}
	vector_reverse_iterator& operator-=(difference_type offset)
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "invalid operation");
		AGL_ASSERT(impl::iterator_in_range(m_ptr + offset, m_begin - 1, m_end - 1), "iterator out of range");

		m_ptr += offset;
		return *this;
	}
	reference operator*() const
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin - 1, m_end - 1), "cannot dereference outside-range iterator");

		return *m_ptr;
	}
	pointer operator->() const
	{
		AGL_ASSERT(m_ptr != nullptr, "invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(m_ptr, m_begin - 1, m_end - 1), "cannot dereference outside-range iterator");

		return m_ptr;
	}

private:
	template <typename U, typename W>
	friend class vector;

	template <typename U, typename W>
	friend class vector_iterator;

	template <typename U, typename W>
	friend class vector_reverse_iterator;

	template <typename U, typename W>
	friend bool operator==(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator!=(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator<(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator<=(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator>(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs);

	template <typename U, typename W>
	friend bool operator>=(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs);

private:
	data_pointer m_begin;
	data_pointer m_end;
	data_pointer m_ptr;
};

template <typename T, typename U>
bool operator==(vector_reverse_iterator<T, U> const& lhs, vector_reverse_iterator<T, U> const& rhs)
{
	return lhs.m_ptr == rhs.m_ptr;
}
template <typename T, typename U>
bool operator!=(vector_reverse_iterator<T, U> const& lhs, vector_reverse_iterator<T, U> const& rhs)
{
	return lhs.m_ptr != rhs.m_ptr;
}
template <typename T, typename U>
bool operator<(vector_reverse_iterator<T, U> const& lhs, vector_reverse_iterator<T, U> const& rhs)
{
	return lhs.m_ptr > rhs.m_ptr;
}
template <typename T, typename U>
bool operator<=(vector_reverse_iterator<T, U> const& lhs, vector_reverse_iterator<T, U> const& rhs)
{
	return lhs.m_ptr >= rhs.m_ptr;
}
template <typename T, typename U>
bool operator>(vector_reverse_iterator<T, U> const& lhs, vector_reverse_iterator<T, U> const& rhs)
{
	return lhs.m_ptr < rhs.m_ptr;
}
template <typename T, typename U>
bool operator>=(vector_reverse_iterator<T, U> const& lhs, vector_reverse_iterator<T, U> const& rhs)
{
	return lhs.m_ptr <= rhs.m_ptr;
}
}

namespace std
{
template <typename T, typename TTraits>
struct iterator_traits<agl::vector_iterator<T, TTraits>>
{
	using traits = TTraits;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;
	using iterator_category = random_access_iterator_tag;

};

template <typename T, typename TTraits>
struct iterator_traits<agl::vector_reverse_iterator<T, TTraits>>
{
	using traits = TTraits;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;
	using iterator_category = random_access_iterator_tag;

};
}