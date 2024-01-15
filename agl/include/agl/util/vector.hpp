#pragma once
#include <cstdint>
#include "agl/util/memory/allocator.hpp"
#include "agl/core/debug.hpp"

namespace agl
{
namespace impl
{
template <typename T, typename TVec>
class vector_iterator;
template <typename T, typename TVec>
class vector_const_iterator;
template <typename T, typename TVec>
class vector_reverse_iterator;
template <typename T, typename TVec>
class vector_reverse_const_iterator;
}
template <typename T, typename TAlloc = mem::allocator<T>>
class vector
{
public:
	using self = vector<T, TAlloc>;
	using value_type = typename TAlloc::value_type;
	using pointer = typename TAlloc::pointer;
	using const_pointer = typename TAlloc::const_pointer;
	using reference = typename TAlloc::reference;
	using const_reference = typename TAlloc::const_reference;
	using size_type = typename TAlloc::size_type;
	using difference_type = typename TAlloc::difference_type;
	using iterator = impl::vector_iterator<T, self>;
	using const_iterator = impl::vector_const_iterator<T, self>;
	using reverse_iterator = impl::vector_reverse_iterator<T, self>;
	using reverse_const_iterator = impl::vector_reverse_const_iterator<T, self>;
	
	vector(TAlloc alloc = TAlloc{}) noexcept
		: m_allocator{ alloc }
		, m_capacity{ 0 }
		, m_memory{ nullptr }
		, m_size{ 0 }
	{
	}

	template <typename TInputIt>
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
	{
		reserve(other.size());
		for (auto const& v : other)
			push_back(v);
	}
	vector& operator=(vector&& other) noexcept
	{
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
		return *(m_memory + index);
	}
	const_reference at() const noexcept
	{
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
	//template <typename TInputIt>
	//void assign(TInputIt first, TInputIt last) noexcept
	//{
	//	auto const count = last - first;
	//	if (count != capacity())
	//	{
	//		clear();
	//		reserve(count);
	//		for (auto i = 0; i < count; ++i, ++first)
	//			push_back(*first);
	//	}
	//	else
	//	{
	//		for (auto i = 0; i < count; ++i, ++first)
	//			*(m_memory + i) = *first;
	//	}
	//}
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
		return reverse_iterator{ m_memory };
	}
	reverse_const_iterator crbegin() const noexcept
	{
		return reverse_const_iterator{ m_memory };
	}
	reverse_iterator rend() const noexcept
	{
		return reverse_iterator{ m_memory + m_size };
	}
	reverse_const_iterator crend() const noexcept
	{
		return reverse_const_iterator{ m_memory + m_size };
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
		return *m_memory;
	}
	const_reference front() const noexcept
	{
		return *m_memory;
	}
	reference back() noexcept
	{
		return *(m_memory + m_size - 1);
	}
	const_reference back() const noexcept
	{
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

	TAlloc get_allocator() const noexcept
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
		if (pos == cend())
		{
			push_back(value);
			return iterator{ m_memory + m_size - 1 };
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		move_elements(index, 1);
		*(m_memory + index) = value;
		++m_size;
		return iterator{ m_memory + index };
	}
	iterator insert(const_iterator pos, value_type&& value) noexcept
	{
		if (pos == cend())
		{
			push_back(std::move(value));
			return iterator{ m_memory + m_size - 1 };
		}
		auto const index = pos - begin();
		reserve(size() + 1);
		move_elements(cbegin() + index, 1);
		*(m_memory + index) = std::move(value);
		++m_size;
		return iterator{ m_memory + index };
	}
	template <typename TInputIt>
	iterator insert(const_iterator pos, TInputIt first, TInputIt last) noexcept
	{
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
		move_elements(cbegin() + index, insert_size);
		for (auto i = difference_type{ 0 }; i < insert_size; ++i)
			*(m_memory + index + i) = first++;
		m_size += insert_size;
		return iterator{ m_memory + index };
	}
	iterator erase(const_iterator pos) noexcept
	{
		m_allocator.destruct(m_memory + (pos - begin()));
		move_elements(pos + 1, -1);
		--m_size;
		return iterator{ m_memory + (pos - begin()) };
	}
	iterator erase(const_iterator first, const_iterator last) noexcept
	{
		auto const erase_size = last - first;
		auto const offset = first - begin();
		for (auto i = difference_type{ 0 }; i < erase_size; ++i)
			m_allocator.destruct(m_memory + offset + i);
		m_size -= erase_size;
		return iterator{ m_memory + offset };
	}
	template <typename TEnable = std::enable_if_t<std::is_move_assignable_v<T>>>
	void push_back(value_type&& value) noexcept
	{
		resize(size() + 1);
		*(m_memory + size() - 1) = std::move(value);
	}

	template <typename TEnable = std::enable_if_t<std::is_copy_assignable_v<T>>>
	void push_back(const_reference value) noexcept
	{
		resize(size() + 1);
		*(m_memory + size() - 1) = value;
	}
	void pop_back() noexcept
	{
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
	/// Moves element under 'pos' and all the elements on it's right 'count' spaces in given direction.
	/// </summary>
	/// <param name="pos">position where to start moving items from</param>
	/// <param name="count">how many spaces will items be moved. if negative, items right to pos will be moved towards the beginning of the array</param>
	void move_elements(const_iterator pos, size_type count) noexcept
	{
		auto const size = pos - begin();
		for (auto i = difference_type{ 0 }; i < size; ++i)
			*(m_memory + size + i) = std::move(*(m_memory + m_size - 1 + count + i));
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
	TAlloc m_allocator;
	size_type m_capacity;
	pointer m_memory;
	size_type m_size;
};
namespace impl
{
template <typename T, typename TVec>
class vector_reverse_iterator
{
public:
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using const_pointer = typename TVec::const_pointer;
	using reference = typename TVec::reference;
	using const_reference = typename TVec::const_reference;
	using size_type = typename TVec::size_type;
	using difference_type = typename TVec::difference_type;

	vector_reverse_iterator() noexcept
		: m_ptr{ nullptr }
	{}
	vector_reverse_iterator(pointer ptr) noexcept
		: m_ptr{ ptr }
	{}
	vector_reverse_iterator(vector_reverse_iterator&& other) noexcept = default;
	vector_reverse_iterator(vector_reverse_iterator const& other) noexcept = default;
	vector_reverse_iterator& operator=(vector_reverse_iterator&& other) noexcept = default;
	vector_reverse_iterator& operator=(vector_reverse_iterator const& other) noexcept = default;
	~vector_reverse_iterator() noexcept = default;
	vector_reverse_iterator& operator++() noexcept
	{
		m_ptr -= 1;
		return *this;
	}
	vector_reverse_iterator operator++(int) const noexcept
	{
		auto result = vector_reverse_iterator{ *this };
		return --result;
	}
	vector_reverse_iterator operator+(difference_type offset) const noexcept
	{
		return vector_reverse_iterator{ m_ptr - offset };
	}
	vector_reverse_iterator& operator+=(difference_type offset) const noexcept
	{
		m_ptr -= offset;
		return *this;
	}
	vector_reverse_iterator& operator--() noexcept
	{
		m_ptr += 1;
		return *this;
	}
	vector_reverse_iterator operator--(int) const noexcept
	{
		auto result = vector_reverse_iterator{ *this };
		return ++result;
	}
	difference_type operator-(vector_reverse_iterator rhs) const noexcept
	{
		return std::abs(*reinterpret_cast<std::int64_t*>(m_ptr) - *reinterpret_cast<std::int64_t*>(rhs.m_ptr));
	}
	vector_reverse_iterator operator-(difference_type offset) const noexcept
	{
		return vector_reverse_iterator{ m_ptr + offset };
	}
	vector_reverse_iterator& operator-=(difference_type offset) const noexcept
	{
		m_ptr += offset;
		return *this;
	}
	reference operator*() noexcept
	{
		return *m_ptr;
	}
	const_reference operator*() const noexcept
	{
		return *m_ptr;
	}
	pointer const operator->() noexcept
	{
		return m_ptr;
	}
	const_pointer operator->() const noexcept
	{
		return m_ptr;
	}
	bool operator==(vector_reverse_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
	bool operator!=(vector_reverse_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
private:
	template <typename U, typename W>
	friend class reverse_const_iterator;
private:
	pointer m_ptr;
};
template <typename T, typename TVec>
class vector_reverse_const_iterator
{
public:
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using const_pointer = typename TVec::const_pointer;
	using reference = typename TVec::reference;
	using const_reference = typename TVec::const_reference;
	using size_type = typename TVec::size_type;
	using difference_type = typename TVec::difference_type;

	vector_reverse_const_iterator() noexcept
		: m_ptr{ nullptr }
	{}
	vector_reverse_const_iterator(pointer ptr) noexcept
		: m_ptr{ ptr }
	{}
	vector_reverse_const_iterator(vector_reverse_iterator<T, TVec>&& other) noexcept
		: m_ptr{ other.m_ptr }
	{}
	vector_reverse_const_iterator(vector_reverse_iterator<T, TVec> const&) noexcept
		: m_ptr{ other.m_ptr }
	{}
	vector_reverse_const_iterator(vector_reverse_const_iterator&& other) noexcept = default;
	vector_reverse_const_iterator(vector_reverse_const_iterator const& other) noexcept = default;
	vector_reverse_const_iterator& operator=(vector_reverse_const_iterator&& other) noexcept = default;
	vector_reverse_const_iterator& operator=(vector_reverse_const_iterator const& other) noexcept = default;
	~vector_reverse_const_iterator() noexcept = default;
	vector_reverse_const_iterator& operator++() noexcept
	{
		m_ptr -= 1;
		return *this;
	}
	vector_reverse_const_iterator operator++(int) const noexcept
	{
		auto result = vector_reverse_const_iterator{ *this };
		return --result;
	}
	vector_reverse_const_iterator operator+(difference_type offset) const noexcept
	{
		return vector_reverse_const_iterator{ m_ptr - offset };
	}
	vector_reverse_const_iterator& operator+=(difference_type offset) const noexcept
	{
		m_ptr -= offset;
		return *this;
	}
	vector_reverse_const_iterator& operator--() noexcept
	{
		m_ptr += 1;
		return *this;
	}
	vector_reverse_const_iterator operator--(int) const noexcept
	{
		auto result = vector_reverse_const_iterator{ *this };
		return ++result;
	}
	difference_type operator-(vector_reverse_const_iterator rhs) const noexcept
	{
		return std::abs(*reinterpret_cast<std::int64_t*>(m_ptr) - *reinterpret_cast<std::int64_t*>(rhs.m_ptr));
	}
	vector_reverse_const_iterator operator-(difference_type offset) const noexcept
	{
		return vector_reverse_const_iterator{ m_ptr + offset };
	}
	vector_reverse_const_iterator& operator-=(difference_type offset) const noexcept
	{
		m_ptr += offset;
		return *this;
	}
	const_reference operator*() const noexcept
	{
		return *m_ptr;
	}
	const_pointer operator->() const noexcept
	{
		return m_ptr;
	}
	bool operator==(vector_reverse_const_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
	bool operator!=(vector_reverse_const_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
private:
	pointer m_ptr;
};
template <typename T, typename TVec>
class vector_iterator
{
public:
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using const_pointer = typename TVec::const_pointer;
	using reference = typename TVec::reference;
	using const_reference = typename TVec::const_reference;
	using size_type = typename TVec::size_type;
	using difference_type = typename TVec::difference_type;

	vector_iterator() noexcept
		: m_ptr{ nullptr }
	{}
	vector_iterator(pointer ptr) noexcept
		: m_ptr{ ptr }
	{}
	vector_iterator(vector_iterator&& other) noexcept = default;
	vector_iterator(vector_iterator const& other) noexcept = default;
	vector_iterator& operator=(vector_iterator&& other) noexcept = default;
	vector_iterator& operator=(vector_iterator const& other) noexcept = default;
	~vector_iterator() noexcept = default;
	vector_iterator& operator++() noexcept
	{
		m_ptr += 1;
		return *this;
	}
	vector_iterator operator++(int) const noexcept
	{
		auto result = vector_iterator{ *this };
		return ++result;
	}
	vector_iterator operator+(difference_type offset) const noexcept
	{
		return vector_iterator{ m_ptr + offset };
	}
	vector_iterator& operator+=(difference_type offset) noexcept
	{
		m_ptr += offset;
		return *this;
	}
	vector_iterator& operator--() noexcept
	{
		m_ptr -= 1;
		return *this;
	}
	vector_iterator operator--(int) const noexcept
	{
		auto result = vector_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_iterator rhs) const noexcept
	{
		return m_ptr - rhs.m_ptr;
	}
	vector_iterator operator-(difference_type offset) const noexcept
	{
		return vector_iterator{ m_ptr - offset };
	}
	vector_iterator& operator-=(difference_type offset) noexcept
	{
		m_ptr -= offset;
		return *this;
	}
	reference operator*() noexcept
	{
		return *m_ptr;
	}
	const_reference operator*() const noexcept
	{
		return *m_ptr;
	}
	pointer operator->() noexcept
	{
		return m_ptr;
	}
	const_pointer operator->() const noexcept
	{
		return m_ptr;
	}
	bool operator==(vector_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
	bool operator!=(vector_iterator const& other) const noexcept
	{
		return m_ptr != other.m_ptr;
	}
private:
	template <typename U, typename W>
	friend class vector_const_iterator;
private:
	pointer m_ptr;
};
template <typename T, typename TVec>
class vector_const_iterator
{
public:
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using const_pointer = typename TVec::const_pointer;
	using reference = typename TVec::reference;
	using const_reference = typename TVec::const_reference;
	using size_type = typename TVec::size_type;
	using difference_type = typename TVec::difference_type;

	vector_const_iterator() noexcept
		: m_ptr{ nullptr }
		, m_size{ 0 }
	{}
	vector_const_iterator(pointer ptr) noexcept
		: m_ptr{ ptr }
	{}
	vector_const_iterator(vector_iterator<T, TVec> const& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_iterator<T, TVec>&& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_const_iterator&& other) noexcept = default;
	vector_const_iterator(vector_const_iterator const& other) noexcept = default;
	vector_const_iterator& operator=(vector_const_iterator&& other) noexcept = default;
	vector_const_iterator& operator=(vector_const_iterator const& other) noexcept = default;
	~vector_const_iterator() noexcept = default;
	vector_const_iterator& operator++() noexcept
	{
		m_ptr += 1;
		return *this;
	}
	vector_const_iterator operator++(int) const noexcept
	{
		auto result = vector_iterator{ *this };
		return ++result;
	}
	vector_const_iterator operator+(difference_type offset) const noexcept
	{
		return vector_const_iterator{ m_ptr + offset };
	}
	vector_const_iterator& operator+=(difference_type offset) noexcept
	{
		m_ptr += offset;
		return *this;
	}
	vector_const_iterator& operator--() noexcept
	{
		m_ptr -= 1;
		return *this;
	}
	vector_const_iterator operator--(int) const noexcept
	{
		auto result = vector_iterator{ *this };
		return --result;
	}
	difference_type operator-(vector_const_iterator rhs) const noexcept
	{
		return m_ptr - rhs.m_ptr;
	}
	vector_const_iterator operator-(difference_type offset) const noexcept
	{
		return vector_const_iterator{ m_ptr - offset };
	}
	vector_const_iterator& operator-=(difference_type offset) noexcept
	{
		m_ptr -= offset;
		return *this;
	}
	const_reference operator*() const noexcept
	{
		return *m_ptr;
	}
	const_pointer operator->() const noexcept
	{
		return m_ptr;
	}
	bool operator==(vector_const_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
	bool operator!=(vector_const_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
private:
	pointer m_ptr;
};
}
}

namespace std
{
template <typename T, typename TVec>
struct iterator_traits<agl::impl::vector_iterator<T, TVec>>
{
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::pointer;
	using reference = typename TVec::reference;
	using difference_type = typename TVec::difference_type;
	using iterator_category = random_access_iterator_tag;

};

template <typename T, typename TVec>
struct iterator_traits<agl::impl::vector_const_iterator<T, TVec>>
{
	using value_type = typename TVec::value_type;
	using pointer = typename TVec::const_pointer;
	using reference = typename TVec::const_reference;
	using difference_type = typename TVec::difference_type;
	using iterator_category = random_access_iterator_tag;
};
}