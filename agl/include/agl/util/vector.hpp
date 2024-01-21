#pragma once
#include <cstdint>
#include "agl/core/debug.hpp"
#include "agl/util/iterator.hpp"
#include "agl/util/memory/allocator.hpp"

namespace agl
{
namespace impl
{
template <typename T>
class vector_iterator;
template <typename T>
class vector_const_iterator;
}
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

	using iterator = vector_iterator<T>;
	using const_iterator = vector_const_iterator<T>;
	using reverse_iterator = std::reverse_iterator<vector_iterator<T>>;
	using reverse_const_iterator = std::reverse_iterator<vector_const_iterator<T>>;
	
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
		return iterator{ m_memory };
	}
	const_iterator cbegin() const noexcept
	{
		return const_iterator{ m_memory };
	}
	iterator end() const noexcept
	{
		return iterator{ m_memory + m_size };
	}
	const_iterator cend() const noexcept
	{
		return const_iterator{ m_memory + m_size };
	}
	reverse_iterator rbegin() const noexcept
	{
		return reverse_iterator{ end()};
	}
	reverse_const_iterator crbegin() const noexcept
	{	
		return reverse_const_iterator{ cend() };
	}
	reverse_iterator rend() const noexcept
	{	
		return reverse_iterator{ begin()};
	}
	reverse_const_iterator crend() const noexcept
	{
		return reverse_const_iterator{ cbegin() };
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
			return iterator{ m_memory + m_size - 1 };
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		move_elements(pos.m_ptr + 1, pos.m_ptr);
		*(m_memory + index) = value;
		++m_size;
		return iterator{ m_memory + index };
	}
	iterator insert(const_iterator pos, value_type&& value) noexcept
	{
		AGL_ASSERT(cbegin() <= pos && pos <= cend(), "Index out of bounds");

		if (pos == cend())
		{
			push_back(std::move(value));
			return iterator{ m_memory + m_size - 1 };
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		move_elements(pos.m_ptr + 1, pos.m_ptr);
		move_elements_right(cbegin() + index, 1);
		*(m_memory + index) = std::move(value);
		++m_size;
		return iterator{ m_memory + index };
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
			return iterator{ m_memory + index };
		}
		reserve(size() + insert_size);
		move_elements(pos.m_ptr, pos.m_ptr + insert_size));
		for (auto i = difference_type{ 0 }; i < insert_size; ++i)
			*(m_memory + index + i) = first++;
		m_size += insert_size;
		return iterator{ m_memory + index };
	}
	iterator erase(const_iterator pos) noexcept
	{
		AGL_ASSERT(cbegin() <= pos && pos < cend(), "Iterator out of bounds");

		m_allocator.destruct(&(*pos));
		m_allocator.construct(&(*pos));
		move_elements(pos.m_ptr, pos.m_ptr + 1);
		--m_size;
		return iterator{ pos.m_ptr };
	}
	iterator erase(const_iterator first, const_iterator last) noexcept
	{
		AGL_ASSERT(cbegin() <= first && first < cend(), "Iterator out of bounds");
		AGL_ASSERT(cbegin() <= last && last < cend(), "Iterator out of bounds");

		for (first; first != last; ++first)
		{
			m_allocator.destruct(first.m_ptr);
			m_allocator.construct(first.m_ptr);
		}
		
		move_elements(first.m_ptr, last.m_ptr)
		m_size -= erase_size;
		return iterator{ m_memory + offset };
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
	/// <summary>
	/// Moves element 'from' and all the elements right to it to the position 'where'
	/// </summary>
	void move_elements(iterator where, iterator from) noexcept
	{
		AGL_ASSERT(cbegin() <= where && where < cend(), "Index out of bounds");
		AGL_ASSERT(cbegin() <= from && from < cend(), "Index out of bounds");
		if (where > from)
			std::swap(where, from);

		for (where, from; from != end(); ++where, ++from)
			*where = std::move(*from);
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
template <typename T>
class vector_iterator
{
public:
	using traits = std::iterator_traits<vector_iterator<T>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	vector_iterator() noexcept
		: m_ptr{ nullptr }
	{
	}
	vector_iterator(pointer ptr) noexcept
		: m_ptr{ ptr }
	{
	}
	vector_iterator(vector_iterator&& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_iterator(vector_iterator const& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_iterator& operator=(vector_iterator&& other) noexcept
	{
		m_ptr = other.m_ptr;
		return *this;
	}
	vector_iterator& operator=(vector_iterator const& other) noexcept
	{
		m_ptr = other.m_ptr;
		return *this;
	}
	~vector_iterator() noexcept
	{
	}
	vector_iterator& operator++() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");

		m_ptr += 1;
		return *this;
	}
	vector_iterator operator++(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");

		auto result = vector_iterator{ *this };
		return ++result;
	}
	vector_iterator operator+(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");

		return vector_iterator{ m_ptr + offset };
	}
	vector_iterator& operator+=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");

		m_ptr += offset;
		return *this;
	}
	vector_iterator& operator--() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");

		m_ptr -= 1;
		return *this;
	}
	vector_iterator operator--(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");

		auto result = vector_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_iterator rhs) const noexcept
	{
		AGL_ASSERT(!((m_ptr == nullptr || rhs.m_ptr == nullptr) && m_ptr != rhs.m_ptr), "Invalid iterator");

		return m_ptr - rhs.m_ptr;
	}
	vector_iterator operator-(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");

		return vector_iterator{ m_ptr - offset };
	}
	vector_iterator& operator-=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");

		m_ptr -= offset;
		return *this;
	}
	reference operator*() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");

		return *m_ptr;
	}
	pointer operator->() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");

		return m_ptr;
	}

private:
	template <typename U>
	friend class vector_const_iterator;

	template <typename U, typename W>
	friend class vector;

	template <typename U>
	friend bool operator==(vector_iterator<U> const& lhs, vector_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator!=(vector_iterator<U> const& lhs, vector_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator<(vector_iterator<U> const& lhs, vector_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator<=(vector_iterator<U> const& lhs, vector_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator>(vector_iterator<U> const& lhs, vector_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator>=(vector_iterator<U> const& lhs, vector_iterator<U> const& rhs) noexcept;

private:
	pointer m_ptr;
};
template <typename T>
bool operator==(vector_iterator<T> const& lhs, vector_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr == rhs.m_ptr;
}
template <typename T>
bool operator!=(vector_iterator<T> const& lhs, vector_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr != rhs.m_ptr;
}
template <typename T>
bool operator<(vector_iterator<T> const& lhs, vector_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr < rhs.m_ptr;
}
template <typename T>
bool operator<=(vector_iterator<T> const& lhs, vector_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr <= rhs.m_ptr;
}
template <typename T>
bool operator>(vector_iterator<T> const& lhs, vector_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr > rhs.m_ptr;
}
template <typename T>
bool operator>=(vector_iterator<T> const& lhs, vector_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr >= rhs.m_ptr;
}

template <typename T>
class vector_const_iterator
{
public:
	using traits = std::iterator_traits<vector_const_iterator<T>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	vector_const_iterator() noexcept
		: m_ptr{ nullptr }
		, m_size{ 0 }
	{
	}
	vector_const_iterator(T* ptr) noexcept
		: m_ptr{ ptr }
	{
	}
	vector_const_iterator(vector_iterator<T> const& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_iterator<T>&& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_const_iterator&& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_const_iterator const& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator& operator=(vector_const_iterator&& other) noexcept
	{
		m_ptr = other.m_ptr;
		return *this;
	}
	vector_const_iterator& operator=(vector_const_iterator const& other) noexcept
	{
		m_ptr = other.m_ptr;
		return *this;
	}
	~vector_const_iterator() noexcept
	{
	}
	vector_const_iterator& operator++() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");

		m_ptr += 1;
		return *this;
	}
	vector_const_iterator operator++(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");

		auto result = vector_const_iterator{ *this };
		return ++result;
	}
	vector_const_iterator operator+(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");

		return vector_const_iterator{ m_ptr + offset };
	}
	vector_const_iterator& operator+=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");

		m_ptr += offset;
		return *this;
	}
	vector_const_iterator& operator--() noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");

		m_ptr -= 1;
		return *this;
	}
	vector_const_iterator operator--(int) const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid operation");

		auto result = vector_const_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_const_iterator rhs) const noexcept
	{
		AGL_ASSERT(!((m_ptr == nullptr || rhs.m_ptr == nullptr) && m_ptr != rhs.m_ptr), "Invalid iterator");

		return m_ptr - rhs.m_ptr;
	}
	vector_const_iterator operator-(difference_type offset) const noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");

		return vector_const_iterator{ m_ptr - offset };
	}
	vector_const_iterator& operator-=(difference_type offset) noexcept
	{
		AGL_ASSERT(!(m_ptr == nullptr && offset != 0), "Invalid operation");

		m_ptr -= offset;
		return *this;
	}
	reference operator*() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");

		return *m_ptr;
	}
	pointer operator->() const noexcept
	{
		AGL_ASSERT(m_ptr != nullptr, "Invalid iterator");

		return m_ptr;
	}

private:
	template <typename U, typename W>
	friend class vector;

	template <typename U>
	friend bool operator==(vector_const_iterator<U> const& lhs, vector_const_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator!=(vector_const_iterator<U> const& lhs, vector_const_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator<(vector_const_iterator<U> const& lhs, vector_const_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator<=(vector_const_iterator<U> const& lhs, vector_const_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator>(vector_const_iterator<U> const& lhs, vector_const_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator>=(vector_const_iterator<U> const& lhs, vector_const_iterator<U> const& rhs) noexcept;

private:
	T* m_ptr;
};
template <typename T>
bool operator==(vector_const_iterator<T> const& lhs, vector_const_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr == rhs.m_ptr;
}
template <typename T>
bool operator!=(vector_const_iterator<T> const& lhs, vector_const_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr != rhs.m_ptr;
}
template <typename T>
bool operator<(vector_const_iterator<T> const& lhs, vector_const_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr < rhs.m_ptr;
}
template <typename T>
bool operator<=(vector_const_iterator<T> const& lhs, vector_const_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr <= rhs.m_ptr;
}
template <typename T>
bool operator>(vector_const_iterator<T> const& lhs, vector_const_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr > rhs.m_ptr;
}
template <typename T>
bool operator>=(vector_const_iterator<T> const& lhs, vector_const_iterator<T> const& rhs) noexcept
{
	return lhs.m_ptr >= rhs.m_ptr;
}
}

namespace std
{
template <typename T>
struct iterator_traits<agl::vector_iterator<T>>
{
	using value_type = typename ::agl::type_traits<T>::value_type;
	using pointer = typename ::agl::type_traits<T>::pointer;
	using reference = typename ::agl::type_traits<T>::reference;
	using difference_type = typename ::agl::type_traits<T>::difference_type;
	using iterator_category = random_access_iterator_tag;

};
template <typename T>
struct iterator_traits<agl::vector_const_iterator<T>>
{
	using value_type = typename ::agl::type_traits<T>::value_type;
	using pointer = typename ::agl::type_traits<T>::pointer;
	using reference = typename ::agl::type_traits<T>::reference;
	using difference_type = typename ::agl::type_traits<T>::difference_type;
	using iterator_category = random_access_iterator_tag;
};
}