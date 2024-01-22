#pragma once
#include <cstdint>
#include "agl/core/debug.hpp"
#include "agl/util/iterator.hpp"
#include "agl/util/memory/allocator.hpp"

namespace agl
{
template <typename T, typename TVec>
class vector_iterator;

template <typename T, typename TVec>
class vector_const_iterator;

template <typename T, typename TVec>
class vector_reverse_iterator;

template <typename T, typename TVec>
class vector_reverse_const_iterator;

template <typename T, typename TAlloc = mem::allocator<T>>
class vector
{
public:
	using allocator_type = typename TAlloc::template rebind<T>;
	using value_type = typename type_traits<T>::value_type;
	using self = vector<typename value_type, typename allocator_type>;
	using pointer = typename type_traits<T>::pointer;
	using const_pointer = typename type_traits<T>::const_pointer;
	using reference = typename type_traits<T>::reference;
	using const_reference = typename type_traits<T>::const_reference;
	using size_type = typename type_traits<T>::size_type;
	using difference_type = typename type_traits<T>::difference_type;

	using iterator = vector_iterator<value_type, self>;
	using const_iterator = vector_const_iterator<value_type, self>;
	using reverse_iterator = vector_reverse_iterator<value_type, self>;
	using reverse_const_iterator = vector_reverse_const_iterator<value_type, self>;
	
	vector() noexcept
		: m_allocator{ }
		, m_capacity{ 0 }
		, m_memory{ nullptr }
		, m_size{ 0 }
	{
	}
	explicit vector(allocator_type const& alloc) noexcept
		: m_allocator{ alloc }
		, m_capacity{ 0 }
		, m_memory{ nullptr }
		, m_size{ 0 }
	{
	}
	template <typename TInputIt, typename TEnable = impl::is_iterator<TInputIt>>
	vector(TInputIt first, TInputIt last) noexcept
	{
		assign(first, last);
	}
	vector(std::initializer_list<T> list) noexcept
	{
		assign(list.begin(), list.end());
	}
	vector(vector&& other) noexcept
		: m_allocator{ std::move(other.m_allocator) }
		, m_capacity{ other.m_capacity }
		, m_memory{ other.m_memory }
		, m_size{ other.m_size }
	{
		other.m_capacity = 0;
		other.m_size = 0;
		other.m_memory = nullptr;
	}
	vector(vector const& other) noexcept
		: m_allocator{ std::move(other.m_allocator) }
		, m_capacity{ 0 }
		, m_memory{ nullptr }
		, m_size{ 0 }
	{
		if (other.m_memory == nullptr)
			return;

		reserve(other.size());
		for (auto const& v : other)
			push_back(v);
	}
	vector& operator=(vector&& other) noexcept
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
	vector& operator=(vector const& other) noexcept
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
	reference at(size_type index) noexcept
	{
		AGL_ASSERT(index < size(), "Index out of bounds");
		AGL_ASSERT(m_memory != nullptr, "Index out of bounds");

		return *(m_memory + index);
	}
	const_reference at() const noexcept
	{
		AGL_ASSERT(index < size(), "Index out of bounds");
		AGL_ASSERT(m_memory != nullptr, "Index out of bounds");
		
		return *(m_memory + index);
	}
	void assign(size_type count, const_reference value) noexcept
	{
		if (count != capacity())
		{
			clear();
			reserve(count);
			for (auto i = 0; i < count; ++i)
				push_back(value);
		}
		else
		{
			for (auto i = 0; i < count; ++i)
				*(m_memory + i) = value;
		}
	}
	template <typename TInputIt, typename TEnable = impl::is_iterator_t<TInputIt>>
	void assign(TInputIt first, TInputIt last) noexcept
	{
		auto const count = last - first;
		if (count != capacity())
		{
			clear();
			reserve(count);
			for (auto i = 0; i < count; ++i, ++first)
				push_back(std::move(*first));
		}
		else
		{
			for (auto i = 0; i < count; ++i, ++first)
				*(m_memory + i) = std::move(*first);
		}
	}
	iterator begin() const noexcept
	{
		return make_iterator<iterator>(m_memory);
	}
	const_iterator cbegin() const noexcept
	{
		return make_iterator<const_iterator>(m_memory);
	}
	iterator end() const noexcept
	{
		return make_iterator<iterator>(m_memory + size());
	}
	const_iterator cend() const noexcept
	{
		return make_iterator<const_iterator>(m_memory + size());
	}
	reverse_iterator rbegin() const noexcept
	{
		if (empty())
			return make_iterator<reverse_iterator>(nullptr);
		return make_iterator<reverse_const_iterator>(cend() - 1);
	}
	reverse_const_iterator crbegin() const noexcept
	{
		if (empty())
			return make_iterator<reverse_const_iterator>(nullptr);
		return make_iterator<reverse_const_iterator>(cend() - 1);
	}
	reverse_iterator rend() const noexcept
	{	
		if (empty())
			return make_iterator<reverse_iterator>(nullptr);
		return make_iterator<reverse_iterator>(begin() - 1);
	}
	reverse_const_iterator crend() const noexcept
	{
		if (empty())
			return make_iterator<reverse_const_iterator>(nullptr);
		return make_iterator<reverse_const_iterator>(cbegin() - 1);
	}
	reference operator[](size_type index) noexcept
	{
		return *(m_memory + index);
	}
	const_reference operator[](size_type index) const noexcept
	{
		return *(m_memory + index);
	}
	reference front() noexcept
	{
		AGL_ASSERT(!empty(), "Index out of bounds");

		return *m_memory;
	}
	const_reference front() const noexcept
	{
		AGL_ASSERT(!empty(), "Index out of bounds");
		
		return *m_memory;
	}
	reference back() noexcept
	{
		AGL_ASSERT(!empty(), "Index out of bounds");
		
		return *(m_memory + m_size - 1);
	}
	const_reference back() const noexcept
	{
		AGL_ASSERT(!empty(), "Index out of bounds");
		
		return *(m_memory + m_size - 1);
	}
	pointer data() noexcept
	{
		return m_memory;
	}
	const_pointer data() const noexcept
	{
		return m_memory
	}
	bool empty() const noexcept
	{
		return m_size == 0;
	}
	allocator_type get_allocator() const noexcept
	{
		return m_allocator;
	}
	size_type size() const noexcept
	{
		return m_size;
	}
	void resize(size_type n) noexcept
	{
		if (n > capacity())
			reserve(n);
		m_size = n;
	}
	size_type capacity() const noexcept
	{
		return m_capacity;
	}
	void shrink_to_fit() noexcept
	{
		// ???
		if (capacity() == size())
			return;
		realloc(size());
	}
	void clear() noexcept
	{
		if (m_memory == nullptr)
			return;

		for (auto i = difference_type{ 0 }; i < size(); ++i)
			m_allocator.destruct(m_memory + i);
		m_allocator.deallocate(m_memory, m_capacity);
		m_memory = nullptr;
		m_size = 0;
		m_capacity = 0;
	}
	iterator insert(const_iterator pos, const_reference value) noexcept
	{
		AGL_ASSERT(cbegin() <= pos && pos <= cend(), "Index out of bounds");

		if (pos == cend())
		{
			push_back(value);
			return make_iterator<iterator>(m_memory + m_size - 1);
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		move_elements_right(begin() + index + 1, begin() + index);
		*(m_memory + index) = value;
		++m_size;
		return make_iterator<iterator>(m_memory + index);
	}
	iterator insert(const_iterator pos, value_type&& value) noexcept
	{
		AGL_ASSERT(cbegin() <= pos && pos <= cend(), "Index out of bounds");

		if (pos == cend())
		{
			push_back(std::move(value));
			return make_iterator<iterator>(m_memory + m_size - 1);
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		move_elements_right(begin() + index + 1, begin() + index);
		*(m_memory + index) = std::move(value);
		++m_size;
		return make_iterator<iterator>(m_memory + index);
	}
	template <typename TInputIt, typename TEnable = impl::is_iterator<TInputIt>>
	iterator insert(const_iterator pos, TInputIt first, TInputIt last) noexcept
	{
		AGL_ASSERT(cbegin() <= pos && pos <= cend(), "Index out of bounds");
		AGL_ASSERT(cbegin() <= first && first <= cend(), "Index out of bounds");
		AGL_ASSERT(cbegin() <= last && last <= cend(), "Index out of bounds");

		auto const index = pos - begin();
		auto const insert_size = std::distance(last - first);
		if (pos == cend())
		{
			reserve(size() + insert_size);
			for(auto it = first; it != last; ++it)
				push_back(*it);
			return make_iterator<iterator>(m_memory + index);
		}
		reserve(size() + insert_size);
		move_elements_right(begin() + index + insert_size, begin() + index);
		for (auto i = difference_type{ 0 }; i < insert_size; ++i)
			*(m_memory + index + i) = first++;
		m_size += insert_size;
		return make_iterator<iterator>(m_memory + index);
	}
	iterator erase(const_iterator pos) noexcept
	{
		AGL_ASSERT(cbegin() <= pos && pos < cend(), "Iterator out of bounds");

		if (size() == 1)
		{
			clear();
			return end();
		}

		m_allocator.destruct(&(*pos));
		m_allocator.construct(&(*pos));
		move_elements_left(pos.m_ptr, pos.m_ptr + 1);
		--m_size;
		return make_iterator<iterator>(pos.m_ptr);
	}
	iterator erase(const_iterator first, const_iterator last) noexcept
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
			m_allocator.destruct(first.m_ptr);
			m_allocator.construct(first.m_ptr);
		}
		
		move_elements_left(first.m_ptr, last.m_ptr)
		m_size -= erase_size;
		return make_iterator<iterator>(m_memory + offset);
	}
	void push_back(value_type&& value) noexcept
	{
		resize(size() + 1);
		*(m_memory + size() - 1) = std::move(value);
	}
	void push_back(const_reference value) noexcept
	{
		resize(size() + 1);
		*(m_memory + size() - 1) = value;
	}
	void pop_back() noexcept
	{
		AGL_ASSERT(!empty(), "erase on empty vector");

		m_allocator.destruct(m_memory + size() - 1);
		--m_size;
	}
	void reserve(size_type n) noexcept
	{
		if (n <= capacity())
			return;
		realloc(n);
	}
private:
	template <typename TIterator>
	TIterator make_iterator(T* ptr) const noexcept
	{
		return TIterator{ ptr, this };
	}
	iterator real_end() const noexcept
	{
		return iterator{ m_memory + capacity() };
	}
	/// <summary>
	/// Moves element 'from' and all the elements right to it to the position 'where'
	/// </summary>
	void move_elements_left(iterator where, iterator from) noexcept
	{
		AGL_ASSERT(where <= from, "Invalid data");
		AGL_ASSERT(begin() <= where && where < end(), "Index out of bounds");
		AGL_ASSERT(begin() <= from && from <= end(), "Index out of bounds");

		for (where, from; from != end(); ++where, ++from)
			*where = std::move(*from);
	}
	void move_elements_right(iterator where, iterator from) noexcept
	{
		AGL_ASSERT(where >= from, "Invalid data");
		AGL_ASSERT(begin() <= where && where < real_end(), "Index out of bounds");
		AGL_ASSERT(begin() <= from && from <= real_end(), "Index out of bounds");

		auto const size = where - from;
		auto w = rbegin() - size; // before begin
		auto f = rbegin();
		auto const end = std::make_reverse_iterator(where);

		for (w, f; w != end; ++w, ++f)
			*w = std::move(*f);
	}
	void realloc(size_type n) noexcept
	{
		auto* tmp_buffer = m_allocator.allocate(n);

		// move current content to new buffer
		for (auto i = difference_type{ 0 }; i < static_cast<difference_type>(size()); ++i)
			m_allocator.construct(tmp_buffer + i, std::move(*(m_memory + i)));

		for (auto i = size(); i < n; ++i)
			m_allocator.construct(tmp_buffer + i);

		m_allocator.deallocate(m_memory, m_capacity);
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
bool iterator_in_range(T const& it, T const& begin, T const& end) noexcept
{
	return begin <= it && it <= end;
}
}

template <typename T, typename TVec>
class vector_iterator
{
public:
	using traits = std::iterator_traits<vector_iterator<T, TVec>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using parent_pointer = TVec*;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	vector_iterator() noexcept
		: m_parent{ nullptr }
		, m_ptr{ nullptr }
	{
	}
	vector_iterator(pointer ptr, parent_pointer parent) noexcept
		: m_parent{ parent }
		, m_ptr{ ptr }

	{
	}
	vector_iterator(vector_iterator&& other) noexcept
		: m_parent{ parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_iterator(vector_iterator const& other) noexcept
		: m_parent{ parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_iterator& operator=(vector_iterator&& other) noexcept
	{
		m_ptr = other.m_ptr;
		m_parent = other.m_parent;
		return *this;
	}
	vector_iterator& operator=(vector_iterator const& other) noexcept
	{
		m_ptr = other.m_ptr;
		m_parent = other.m_parent;
		return *this;
	}
	~vector_iterator() noexcept
	{
	}
	vector_iterator& operator++() noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->begin(), m_parent->end() - 1), "Invalid iterator");

		m_ptr += 1;
		return *this;
	}
	vector_iterator operator++(int) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->begin(), m_parent->end() - 1), "Invalid iterator");

		auto result = vector_iterator{ *this };
		return ++result;
	}
	vector_iterator operator+(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->begin(), m_parent->end()), "Invalid iterator");

		return vector_iterator{ m_ptr + offset, m_parent };
	}
	vector_iterator& operator+=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->begin(), m_parent->end()), "Invalid iterator");

		m_ptr += offset;
		return *this;
	}
	vector_iterator& operator--() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->begin() + 1, m_parent->end()), "Invalid iterator");

		m_ptr -= 1;
		return *this;
	}
	vector_iterator operator--(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->begin() + 1, m_parent->end()), "Invalid iterator");

		auto result = vector_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_iterator rhs) const noexcept
	{
		AGL_ASSERT(!((m_ptr == nullptr || rhs.m_ptr == nullptr)), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->begin() + 1, m_parent->end()), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(rhs.m_ptr, m_parent->begin() + 1, m_parent->end()), "Invalid iterator");

		return m_ptr - rhs.m_ptr;
	}
	vector_iterator operator-(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this - offset, m_parent->begin(), m_parent->end()), "Invalid iterator");

		return vector_iterator{ m_ptr - offset, m_parent };
	}
	vector_iterator& operator-=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this - offset, m_parent->begin(), m_parent->end()), "Invalid iterator");

		m_ptr -= offset;
		return *this;
	}
	reference operator*() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->begin(), m_parent->end()), "Cannot dereference outside-range iterator");

		return *m_ptr;
	}
	pointer operator->() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->begin(), m_parent->end()), "Cannot dereference outside-range iterator");

		return m_ptr;
	}

private:
	template <typename U, typename W>
	friend class vector_const_iterator;

	template <typename U, typename W>
	friend class vector;

	template <typename U, typename W>
	friend bool operator==(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator!=(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator<(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator<=(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator>(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator>=(vector_iterator<U, W> const& lhs, vector_iterator<U, W> const& rhs) noexcept;

private:
	pointer m_ptr;
	vector<T>* m_parent;
};

template <typename T, typename TVec>
bool operator==(vector_iterator<T, TVec> const& lhs, vector_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr == rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator!=(vector_iterator<T, TVec> const& lhs, vector_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr != rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator<(vector_iterator<T, TVec> const& lhs, vector_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr < rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator<=(vector_iterator<T, TVec> const& lhs, vector_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr <= rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator>(vector_iterator<T, TVec> const& lhs, vector_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr > rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator>=(vector_iterator<T, TVec> const& lhs, vector_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr >= rhs.m_ptr;
}

template <typename T, typename TVec>
class vector_const_iterator
{
public:
	using traits = std::iterator_traits<vector_const_iterator<T, TVec>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using parent_pointer = TVec*;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	vector_const_iterator() noexcept
		: m_parent{ nullptr }
		, m_ptr{ nullptr }
		, m_size{ 0 }
	{
	}
	vector_const_iterator(pointer ptr, parent_pointer parent) noexcept
		: m_parent{ parent }
		, m_ptr{ ptr }
	{
	}
	vector_const_iterator(vector_iterator<T, TVec> const& other) noexcept
		: m_parent{ other.m_parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_iterator<T, TVec>&& other) noexcept
		: m_parent{ other.m_parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_const_iterator&& other) noexcept
		: m_parent{ other.m_parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_const_iterator const& other) noexcept
		: m_parent{ other.m_parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator& operator=(vector_const_iterator&& other) noexcept
	{
		m_parent = other.m_parent;
		m_ptr = other.m_ptr;
		return *this;
	}
	vector_const_iterator& operator=(vector_const_iterator const& other) noexcept
	{
		m_parent = other.m_parent;
		m_ptr = other.m_ptr;
		return *this;
	}
	~vector_const_iterator() noexcept
	{
	}
	vector_const_iterator& operator++() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->cbegin(), m_parent->cend() - 1), "Invalid iterator");

		m_ptr += 1;
		return *this;
	}
	vector_const_iterator operator++(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->cbegin(), m_parent->cend() - 1), "Invalid iterator");

		auto result = vector_const_iterator{ *this };
		return ++result;
	}
	vector_const_iterator operator+(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->cbegin(), m_parent->cend()), "Invalid iterator");

		return vector_const_iterator{ m_ptr + offset, m_parent };
	}
	vector_const_iterator& operator+=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->cbegin(), m_parent->cend()), "Invalid iterator");

		m_ptr += offset;
		return *this;
	}
	vector_const_iterator& operator--() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->cbegin() + 1, m_parent->cend()), "Invalid iterator");

		m_ptr -= 1;
		return *this;
	}
	vector_const_iterator operator--(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->cbegin() + 1, m_parent->cend()), "Invalid iterator");

		auto result = vector_const_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_const_iterator rhs) const noexcept
	{
		AGL_ASSERT(!((m_ptr == nullptr || rhs.m_ptr == nullptr) && m_ptr != rhs.m_ptr), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->cbegin() + 1, m_parent->cend()), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(rhs.m_ptr, m_parent->cbegin() + 1, m_parent->cend()), "Invalid iterator");

		return m_ptr - rhs.m_ptr;
	}
	vector_const_iterator operator-(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this - offset, m_parent->cbegin(), m_parent->cend()), "Invalid iterator");

		return vector_const_iterator{ m_ptr - offset, m_parent };
	}
	vector_const_iterator& operator-=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->cbegin(), m_parent->cend()), "Invalid iterator");

		m_ptr -= offset;
		return *this;
	}
	reference operator*() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->cbegin(), m_parent->cend()), "Invalid iterator");

		return *m_ptr;
	}
	pointer operator->() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->cbegin(), m_parent->cend()), "Invalid iterator");

		return m_ptr;
	}

private:
	template <typename U, typename W>
	friend class vector;

	template <typename U, typename W>
	friend bool operator==(vector_const_iterator<U, W> const& lhs, vector_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator!=(vector_const_iterator<U, W> const& lhs, vector_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator<(vector_const_iterator<U, W> const& lhs, vector_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator<=(vector_const_iterator<U, W> const& lhs, vector_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator>(vector_const_iterator<U, W> const& lhs, vector_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator>=(vector_const_iterator<U, W> const& lhs, vector_const_iterator<U, W> const& rhs) noexcept;

private:
	parent_pointer m_parent;
	pointer m_ptr;
};

template <typename T, typename TVec>
bool operator==(vector_const_iterator<T, TVec> const& lhs, vector_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr == rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator!=(vector_const_iterator<T, TVec> const& lhs, vector_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr != rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator<(vector_const_iterator<T, TVec> const& lhs, vector_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr < rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator<=(vector_const_iterator<T, TVec> const& lhs, vector_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr <= rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator>(vector_const_iterator<T, TVec> const& lhs, vector_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr > rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator>=(vector_const_iterator<T, TVec> const& lhs, vector_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr >= rhs.m_ptr;
}

template <typename T, typename TVec>
class vector_reverse_iterator
{
public:
	using traits = std::iterator_traits<vector_reverse_iterator<T, TVec>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using parent_pointer = TVec*;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	vector_reverse_iterator() noexcept
		: m_parent{ nullptr }
		, m_ptr{ nullptr }
	{
	}
	vector_reverse_iterator(pointer ptr, parent_pointer parent) noexcept
		: m_parent{ parent }
		, m_ptr{ ptr }

	{
	}
	vector_reverse_iterator(vector_iterator<T, TVec> const& other) noexcept
		: m_parent{ other.m_parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_reverse_iterator(vector_reverse_iterator&& other) noexcept
		: m_parent{ parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_reverse_iterator(vector_reverse_iterator const& other) noexcept
		: m_parent{ parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_reverse_iterator& operator=(vector_reverse_iterator&& other) noexcept
	{
		m_ptr = other.m_ptr;
		m_parent = other.m_parent;
		return *this;
	}
	vector_reverse_iterator& operator=(vector_reverse_iterator const& other) noexcept
	{
		m_ptr = other.m_ptr;
		m_parent = other.m_parent;
		return *this;
	}
	~vector_reverse_iterator() noexcept
	{
	}
	vector_reverse_iterator& operator++() noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->rbegin(), m_parent->rend() - 1), "Invalid iterator");

		m_ptr -= 1;
		return *this;
	}
	vector_reverse_iterator operator++(int) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->rbegin(), m_parent->rend() - 1), "Invalid iterator");

		auto result = vector_reverse_iterator{ *this };
		return ++result;
	}
	vector_reverse_iterator operator+(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this - offset, m_parent->rbegin(), m_parent->rend()), "Invalid iterator");

		return vector_reverse_iterator{ m_ptr - offset };
	}
	vector_reverse_iterator& operator+=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this - offset, m_parent->rbegin(), m_parent->rend()), "Invalid iterator");

		m_ptr -= offset;
		return *this;
	}
	vector_reverse_iterator& operator--() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->rbegin() + 1, m_parent->rend()), "Invalid iterator");

		m_ptr += 1;
		return *this;
	}
	vector_reverse_iterator operator--(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->rbegin() + 1, m_parent->rend()), "Invalid iterator");

		auto result = vector_reverse_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_reverse_iterator rhs) const noexcept
	{
		AGL_ASSERT(!((m_ptr == nullptr || rhs.m_ptr == nullptr)), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->rbegin() + 1, m_parent->rend()), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(rhs.m_ptr, m_parent->rbegin() + 1, m_parent->rend()), "Invalid iterator");

		return std::abs(*reinterpret_cast<std::int64_t*>(&m_ptr) - *reinterpret_cast<std::int64_t*>(&rhs.m_ptr));
	}
	vector_reverse_iterator operator-(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->rbegin(), m_parent->rend()), "Invalid iterator");

		return vector_iterator{ m_ptr + offset, m_parent };
	}
	vector_reverse_iterator& operator-=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->rbegin(), m_parent->rend()), "Invalid iterator");

		m_ptr += offset;
		return *this;
	}
	reference operator*() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->rbegin(), m_parent->rend()), "Cannot dereference outside-range iterator");

		return *m_ptr;
	}
	pointer operator->() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->rbegin(), m_parent->rend()), "Cannot dereference outside-range iterator");

		return m_ptr;
	}

private:
	template <typename U, typename W>
	friend class vector_reverse_const_iterator;

	template <typename U, typename W>
	friend class vector;

	template <typename U, typename W>
	friend bool operator==(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator!=(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator<(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator<=(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator>(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator>=(vector_reverse_iterator<U, W> const& lhs, vector_reverse_iterator<U, W> const& rhs) noexcept;

private:
	pointer m_ptr;
	vector<T>* m_parent;
};

template <typename T, typename TVec>
bool operator==(vector_reverse_iterator<T, TVec> const& lhs, vector_reverse_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr == rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator!=(vector_reverse_iterator<T, TVec> const& lhs, vector_reverse_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr != rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator<(vector_reverse_iterator<T, TVec> const& lhs, vector_reverse_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr > rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator<=(vector_reverse_iterator<T, TVec> const& lhs, vector_reverse_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr >= rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator>(vector_reverse_iterator<T, TVec> const& lhs, vector_reverse_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr < rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator>=(vector_reverse_iterator<T, TVec> const& lhs, vector_reverse_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr <= rhs.m_ptr;
}

template <typename T, typename TVec>
class vector_reverse_const_iterator
{
public:
	using traits = std::iterator_traits<vector_reverse_const_iterator<T, TVec>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using parent_pointer = TVec*;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	vector_reverse_const_iterator() noexcept
		: m_parent{ nullptr }
		, m_ptr{ nullptr }
	{
	}
	vector_reverse_const_iterator(pointer ptr, parent_pointer parent) noexcept
		: m_parent{ parent }
		, m_ptr{ ptr }

	{
	}
	vector_reverse_const_iterator(vector_reverse_iterator<T, TVec> const& other) noexcept
		: m_parent{ other.m_parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_reverse_const_iterator(vector_reverse_const_iterator&& other) noexcept
		: m_parent{ parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_reverse_const_iterator(vector_reverse_const_iterator const& other) noexcept
		: m_parent{ parent }
		, m_ptr{ other.m_ptr }
	{
	}
	vector_reverse_const_iterator& operator=(vector_reverse_const_iterator&& other) noexcept
	{
		m_ptr = other.m_ptr;
		m_parent = other.m_parent;
		return *this;
	}
	vector_reverse_const_iterator& operator=(vector_reverse_const_iterator const& other) noexcept
	{
		m_ptr = other.m_ptr;
		m_parent = other.m_parent;
		return *this;
	}
	~vector_reverse_const_iterator() noexcept
	{
	}
	vector_reverse_const_iterator& operator++() noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->crbegin(), m_parent->crend() - 1), "Invalid iterator");

		m_ptr -= 1;
		return *this;
	}
	vector_reverse_const_iterator operator++(int) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->crbegin(), m_parent->crend() - 1), "Invalid iterator");

		auto result = vector_reverse_const_iterator{ *this };
		return ++result;
	}
	vector_reverse_const_iterator operator+(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this - offset, m_parent->crbegin(), m_parent->crend()), "Invalid iterator");

		return vector_reverse_const_iterator{ m_ptr - offset };
	}
	vector_reverse_const_iterator& operator+=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this - offset, m_parent->crbegin(), m_parent->crend()), "Invalid iterator");

		m_ptr -= offset;
		return *this;
	}
	vector_reverse_const_iterator& operator--() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->crbegin() + 1, m_parent->crend()), "Invalid iterator");

		m_ptr += 1;
		return *this;
	}
	vector_reverse_const_iterator operator--(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->crbegin() + 1, m_parent->crend()), "Invalid iterator");

		auto result = vector_reverse_const_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_reverse_const_iterator rhs) const noexcept
	{
		AGL_ASSERT(!((m_ptr == nullptr || rhs.m_ptr == nullptr)), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->crbegin() + 1, m_parent->crend()), "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(rhs.m_ptr, m_parent->crbegin() + 1, m_parent->crend()), "Invalid iterator");

		return std::abs(*reinterpret_cast<std::int64_t*>(&m_ptr) - *reinterpret_cast<std::int64_t*>(&rhs.m_ptr));
	}
	vector_reverse_const_iterator operator-(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->crbegin(), m_parent->crend()), "Invalid iterator");

		return vector_reverse_const_iterator{ m_ptr + offset };
	}
	vector_reverse_const_iterator& operator-=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");
		AGL_ASSERT(impl::iterator_in_range(*this + offset, m_parent->crbegin(), m_parent->crend()), "Invalid iterator");

		m_ptr += offset;
		return *this;
	}
	reference operator*() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->crbegin(), m_parent->crend()), "Cannot dereference outside-range iterator");

		return *m_ptr;
	}
	pointer operator->() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");
		AGL_ASSERT(impl::iterator_in_range(*this, m_parent->crbegin(), m_parent->crend()), "Cannot dereference outside-range iterator");

		return m_ptr;
	}

private:
	template <typename U, typename W>
	friend class vector_reverse_const_iterator;

	template <typename U, typename W>
	friend class vector;

	template <typename U, typename W>
	friend bool operator==(vector_reverse_const_iterator<U, W> const& lhs, vector_reverse_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator!=(vector_reverse_const_iterator<U, W> const& lhs, vector_reverse_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator<(vector_reverse_const_iterator<U, W> const& lhs, vector_reverse_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator<=(vector_reverse_const_iterator<U, W> const& lhs, vector_reverse_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator>(vector_reverse_const_iterator<U, W> const& lhs, vector_reverse_const_iterator<U, W> const& rhs) noexcept;

	template <typename U, typename W>
	friend bool operator>=(vector_reverse_const_iterator<U, W> const& lhs, vector_reverse_const_iterator<U, W> const& rhs) noexcept;

private:
	pointer m_ptr;
	vector<T>* m_parent;
};

template <typename T, typename TVec>
bool operator==(vector_reverse_const_iterator<T, TVec> const& lhs, vector_reverse_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr == rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator!=(vector_reverse_const_iterator<T, TVec> const& lhs, vector_reverse_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr != rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator<(vector_reverse_const_iterator<T, TVec> const& lhs, vector_reverse_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr > rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator<=(vector_reverse_const_iterator<T, TVec> const& lhs, vector_reverse_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr >= rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator>(vector_reverse_const_iterator<T, TVec> const& lhs, vector_reverse_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr < rhs.m_ptr;
}
template <typename T, typename TVec>
bool operator>=(vector_reverse_const_iterator<T, TVec> const& lhs, vector_reverse_const_iterator<T, TVec> const& rhs) noexcept
{
	return lhs.m_ptr <= rhs.m_ptr;
}
}

namespace std
{
template <typename T, typename TVec>
struct iterator_traits<agl::vector_iterator<T, TVec>>
{
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using reference = typename TVec::reference;
	using difference_type = typename TVec::difference_type;
	using iterator_category = random_access_iterator_tag;

};

template <typename T, typename TVec>
struct iterator_traits<agl::vector_const_iterator<T, TVec>>
{
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using reference = typename TVec::reference;
	using difference_type = typename TVec::difference_type;
	using iterator_category = random_access_iterator_tag;
}; 

template <typename T, typename TVec>
struct iterator_traits<agl::vector_reverse_iterator<T, TVec>>
{
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using reference = typename TVec::reference;
	using difference_type = typename TVec::difference_type;
	using iterator_category = random_access_iterator_tag;

};

template <typename T, typename TVec>
struct iterator_traits<agl::vector_reverse_const_iterator<T, TVec>>
{
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using reference = typename TVec::reference;
	using difference_type = typename TVec::difference_type;
	using iterator_category = random_access_iterator_tag;
};
}