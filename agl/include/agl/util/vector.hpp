#pragma once
#include <cstdint>
#include "agl/util/memory/allocator.hpp"
namespace agl
{
namespace impl
{
template <typename T>
class vector_iterator;
template <typename T>
class vector_const_iterator;
template <typename T>
class reverse_vector_iterator;
template <typename T>
class reverse_vector_const_iterator;
}
template <typename T, typename TAlloc = mem::allocator<T>>
class vector
{
public:
	using iterator = impl::vector_iterator<T>;
	using const_iterator = impl::vector_const_iterator<T>;
	using reverse_iterator = impl::reverse_vector_iterator<T>;
	using reverse_const_iterator = impl::reverse_vector_const_iterator<T>;
	
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
		m_allocator = other.m_allocator;
		m_capacity = other.m_capacity;
		other.m_capacity = 0;
		m_memory = other.m_memory;
		other.m_memory = nullptr;
		m_size = other.m_size;
		other.m_size = 0;
		return *this;
	}
	~vector()
	{
		clear();
	}
	T& at(std::uint64_t index) noexcept
	{
		return *(m_memory + index);
	}
	T const& at() const noexcept
	{
		return *(m_memory + index);
	}
	void assign(std::uint64_t count, T const& value) noexcept
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
	iterator begin() noexcept
	{
		return iterator{ m_memory };
	}
	const_iterator cbegin() const noexcept
	{
		return const_iterator{ m_memory };
	}
	iterator end() noexcept
	{
		return iterator{ m_memory + m_size };
	}
	const_iterator cend() const noexcept
	{
		return const_iterator{ m_memory + m_size };
	}

	reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{ m_memory };
	}
	reverse_const_iterator crbegin() const noexcept
	{
		return reverse_const_iterator{ m_memory };
	}
	reverse_iterator rend() noexcept
	{
		return reverse_iterator{ m_memory + m_size };
	}
	reverse_const_iterator crend() const noexcept
	{
		return reverse_const_iterator{ m_memory + m_size };
	}
	T& operator[](std::uint64_t index) noexcept
	{
		return *(m_memory + index);
	}
	T const& operator[](std::uint64_t index) const noexcept
	{
		return *(m_memory + index);
	}
	T& front() noexcept
	{
		return *m_memory;
	}
	T const& front() const noexcept
	{
		return *m_memory;
	}
	T& back() noexcept
	{
		return *(m_memory + m_size - 1);
	}
	T const& back() const noexcept
	{
		return *(m_memory + m_size - 1);
	}
	T* const data() noexcept
	{
		return m_memory;
	}
	T const* const data() const noexcept
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

	std::uint64_t size() const noexcept
	{
		return m_size;
	}
	void resize(std::uint64_t n) noexcept
	{
		if (n > capacity())
			reserve(n);
		m_size = n;
	}
	std::uint64_t capacity() const noexcept
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
		for (auto i = std::uint64_t{}; i < size(); ++i)
			m_allocator.destruct<T>(m_memory + i);
		m_allocator.deallocate(reinterpret_cast<std::byte*>(m_memory), m_capacity);
		m_memory = nullptr;
		m_size = 0;
		m_capacity = 0;
	}
	iterator insert(const_iterator pos, T const& value) noexcept
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
	iterator insert(const_iterator pos, T&& value) noexcept
	{
		if (pos == cend())
		{
			push_back(value);
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
		for (auto i = 0; i < insert_size; ++i)
			*(m_memory + index + i) = first++;
		m_size += insert_size;
		return iterator{ m_memory + index };
	}
	iterator erase(const_iterator pos) noexcept
	{
		m_allocator.destruct<T>(m_memory + pos - begin());
		move_elements(pos + 1, -1);
		--m_size;
		return iterator{ m_memory + pos };
	}
	iterator erase(const_iterator first, const_iterator last) noexcept
	{
		auto const erase_size = last - first;
		auto const offset = first - begin();
		for (auto i = 0; i < erase_size; ++i)
			m_allocator.destruct<T>(m_memory + offset + i);
		m_size -= erase_size;
		return iterator{ m_memory + offset };
	}
	void push_back(T&& value) noexcept
	{
		resize(size() + 1);
		*(m_memory + size()) = std::move(value);
		++m_size;
	}
	void push_back(T const& value) noexcept
	{
		resize(size() + 1);
		*(m_memory + size()) = value;
		++m_size;
	}
	void pop_back() noexcept
	{
		m_allocator.destruct<T>(m_memory + size() - 1);
		--m_size;
	}
	void reserve(std::uint64_t n) noexcept
	{
		if (n <= capacity())
			return;
		realloc(n);
	}
private:
	/// <summary>
	///
	/// </summary>
	/// <param name="pos">position where to start moving items from</param>
	/// <param name="count">how many spaces will items be moved. if negative, items right to pos will be moved towards the beginning of the array</param>
	void move_elements(const_iterator pos, std::int64_t count) noexcept
	{
		auto const size = pos - begin();
		for (auto i = 0; i < size; ++i)
			*(m_memory + pos + i) = *(m_memory + m_size - 1 + count + i);
		for(auto i = )
	}
	void realloc(std::uint64_t n) noexcept
	{
		auto* tmp_buffer = m_allocator.allocate(n * sizeof(T));
		m_allocator.align<T>(tmp_buffer, n);
		// move current content to new buffer
		for (auto i = std::uint64_t{}; i < capacity(); ++i)
			m_allocator.construct<T>(tmp_buffer + i, *(m_memory + i));
		m_allocator.deallocate(reinterpret_cast<std::byte*>(m_memory), m_capacity);
		m_memory = reinterpret_cast<T*>(tmp_buffer);
		m_capacity = n;
	}
private:
	TAlloc m_allocator;
	std::uint64_t m_capacity;
	T *m_memory;
	std::uint64_t m_size;
};
namespace impl
{
template <typename T>
class reverse_iterator
{
public:
	reverse_iterator() noexcept
		: m_ptr{ nullptr }
	{}
	reverse_iterator(T* ptr) noexcept
		: m_ptr{ ptr }
	{}
	reverse_iterator(reverse_iterator&& other) noexcept = default;
	reverse_iterator(reverse_iterator const& other) noexcept = default;
	reverse_iterator& operator=(reverse_iterator&& other) noexcept = default;
	reverse_iterator& operator=(reverse_iterator const& other) noexcept = default;
	~reverse_iterator() noexcept = default;
	reverse_iterator& operator++() noexcept
	{
		m_ptr -= 1;
		return *this;
	}
	reverse_iterator operator++(int) const noexcept
	{
		auto result = reverse_iterator{ *this };
		return --result;
	}
	reverse_iterator operator+(std::uint64_t offset) const noexcept
	{
		return reverse_iterator{ m_ptr - offset };
	}
	reverse_iterator& operator+=(std::uint64_t offset) const noexcept
	{
		m_ptr -= offset;
		return *this;
	}
	reverse_iterator& operator--() noexcept
	{
		m_ptr += 1;
		return *this;
	}
	reverse_iterator operator--(int) const noexcept
	{
		auto result = reverse_iterator{ *this };
		return ++result;
	}
	std::uint64_t operator-(reverse_iterator rhs) const noexcept
	{
		return std::abs(*reinterpret_cast<std::int64_t*>(m_ptr) - *reinterpret_cast<std::int64_t*>(rhs.m_ptr));
	}
	reverse_iterator operator-(std::uint64_t offset) const noexcept
	{
		return reverse_iterator{ m_ptr + offset };
	}
	reverse_iterator& operator-=(std::uint64_t offset) const noexcept
	{
		m_ptr += offset;
		return *this;
	}
	T& operator*() noexcept
	{
		return *m_ptr;
	}
	T const& operator*() const noexcept
	{
		return *m_ptr;
	}
	T* const operator->() noexcept
	{
		return m_ptr;
	}
	T const* const operator->() const noexcept
	{
		return m_ptr;
	}
	bool operator==(reverse_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
	bool operator!=(reverse_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
private:
	template <typename U>
	friend class reverse_const_iterator;
private:
	T* m_ptr;
};
template <typename T>
class reverse_const_iterator
{
public:
	reverse_const_iterator() noexcept
		: m_ptr{ nullptr }
	{}
	reverse_const_iterator(T* ptr) noexcept
		: m_ptr{ ptr }
	{}
	reverse_const_iterator(reverse_iterator<T>&& other) noexcept
		: m_ptr{ other.m_ptr }
	{}
	reverse_const_iterator(reverse_iterator<T> const&) noexcept
		: m_ptr{ other.m_ptr }
	{}
	reverse_const_iterator(reverse_const_iterator&& other) noexcept = default;
	reverse_const_iterator(reverse_const_iterator const& other) noexcept = default;
	reverse_const_iterator& operator=(reverse_const_iterator&& other) noexcept = default;
	reverse_const_iterator& operator=(reverse_const_iterator const& other) noexcept = default;
	~reverse_const_iterator() noexcept = default;
	reverse_const_iterator& operator++() noexcept
	{
		m_ptr -= 1;
		return *this;
	}
	reverse_const_iterator operator++(int) const noexcept
	{
		auto result = reverse_const_iterator{ *this };
		return --result;
	}
	reverse_const_iterator operator+(std::uint64_t offset) const noexcept
	{
		return reverse_const_iterator{ m_ptr - offset };
	}
	reverse_const_iterator& operator+=(std::uint64_t offset) const noexcept
	{
		m_ptr -= offset;
		return *this;
	}
	reverse_const_iterator& operator--() noexcept
	{
		m_ptr += 1;
		return *this;
	}
	reverse_const_iterator operator--(int) const noexcept
	{
		auto result = reverse_const_iterator{ *this };
		return ++result;
	}
	std::uint64_t operator-(reverse_const_iterator rhs) const noexcept
	{
		return std::abs(*reinterpret_cast<std::int64_t*>(m_ptr) - *reinterpret_cast<std::int64_t*>(rhs.m_ptr));
	}
	reverse_const_iterator operator-(std::uint64_t offset) const noexcept
	{
		return reverse_const_iterator{ m_ptr + offset };
	}
	reverse_const_iterator& operator-=(std::uint64_t offset) const noexcept
	{
		m_ptr += offset;
		return *this;
	}
	T& operator*() noexcept
	{
		return *m_ptr;
	}
	T const& operator*() const noexcept
	{
		return *m_ptr;
	}
	T* const operator->() noexcept
	{
		return m_ptr;
	}
	T const* const operator->() const noexcept
	{
		return m_ptr;
	}
	bool operator==(reverse_const_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
	bool operator!=(reverse_const_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
private:
	T* m_ptr;
};
template <typename T>
class vector_iterator
{
public:
	vector_iterator() noexcept
		: m_ptr{ nullptr }
	{}
	vector_iterator(T* ptr) noexcept
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
	vector_iterator operator+(std::uint64_t offset) const noexcept
	{
		return vector_iterator{ m_ptr + offset };
	}
	vector_iterator& operator+=(std::uint64_t offset) const noexcept
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
	std::uint64_t operator-(vector_iterator rhs) const noexcept
	{
		return *reinterpret_cast<std::uint64_t*>(m_ptr - rhs.m_ptr);
	}
	vector_iterator operator-(std::uint64_t offset) const noexcept
	{
		return vector_iterator{ m_ptr - offset };
	}
	vector_iterator& operator-=(std::uint64_t offset) const noexcept
	{
		m_ptr -= offset;
		return *this;
	}
	T& operator*() noexcept
	{
		return *m_ptr;
	}
	T const& operator*() const noexcept
	{
		return *m_ptr;
	}
	T* const operator->() noexcept
	{
		return m_ptr;
	}
	T const* const operator->() const noexcept
	{
		return m_ptr;
	}
	bool operator==(vector_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
	bool operator!=(vector_iterator const& other) const noexcept
	{
		return m_ptr == other.m_ptr;
	}
private:
	template <typename U>
	friend class vector_const_iterator;
private:
	T* m_ptr;
};
template <typename T>
class vector_const_iterator
{
public:
	vector_const_iterator() noexcept
		: m_ptr{ nullptr }
		, m_size{ 0 }
	{}
	vector_const_iterator(T* ptr) noexcept
		: m_ptr{ ptr }
	{}
	vector_const_iterator(vector_iterator<T> const& other) noexcept
		: m_ptr{ other.m_ptr }
	{
	}
	vector_const_iterator(vector_iterator<T>&& other) noexcept
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
	vector_const_iterator operator+(std::uint64_t offset) const noexcept
	{
		return vector_const_iterator{ m_ptr + offset };
	}
	vector_const_iterator& operator+=(std::uint64_t offset) const noexcept
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
	std::uint64_t operator-(vector_const_iterator rhs) const noexcept
	{
		return *reinterpret_cast<std::uint64_t*>(m_ptr - rhs.m_ptr);
	}
	vector_const_iterator operator-(std::uint64_t offset) const noexcept
	{
		return vector_const_iterator{ m_ptr - offset };
	}
	vector_const_iterator& operator-=(std::uint64_t offset) const noexcept
	{
		m_ptr -= offset;
		return *this;
	}
	T const& operator*() const noexcept
	{
		return *m_ptr;
	}
	T const* const operator->() const noexcept
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
	T* m_ptr;
};
}
}