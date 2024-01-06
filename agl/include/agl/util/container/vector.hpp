#pragma once
#include <cstdint>
#include "agl/util/memory/allocator.hpp"

namespace agl
{
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

	reverse_iterator operator++(std::uint64_t) const noexcept
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

	reverse_iterator operator--(std::uint64_t) const noexcept
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

	template <typename U>
	friend class vector;
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

	reverse_const_iterator(reverse_iterator&& other) noexcept
		: m_ptr{ other.m_ptr }
	{}

	reverse_const_iterator(reverse_iterator const&) noexcept
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

	reverse_const_iterator operator++(std::uint64_t) const noexcept
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

	reverse_const_iterator operator--(std::uint64_t) const noexcept
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
	friend class vector;

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
		: m_ptr { ptr }
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

	vector_iterator operator++(std::uint64_t) const noexcept
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

	vector_iterator operator--(std::uint64_t) const noexcept
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

	template <typename U>
	friend class vector;
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

	vector_const_iterator(vector_iterator const& other) noexcept
		: m_ptr{ other.m_ptr}
	{
	}
	
	vector_const_iterator(vector_iterator&& other) noexcept
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

	vector_const_iterator operator++(std::uint64_t) const noexcept
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

	vector_const_iterator operator--(std::uint64_t) const noexcept
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
	template <typename U>
	friend class vector;

private:
	T* m_ptr;
};
}

template <typename T, typename TAlloc = mem::allocator>
class vector
{
	using iterator = impl::vector_iterator<T>;
	using const_iterator = impl::vector_const_iterator<T>;

	vector(TAlloc alloc = TAlloc{}) noexcept
		: m_memory{ nullptr }
		, m_size{ nullptr }
		, m_allocator{ alloc }
	{}

	vector(vector&& other) noexcept
		: m_memory{ other.m_memory }
		, m_size{ other.m_size }
		, m_allocator{ std::move(other.m_allocator) }
	{
	}

	vector(vector const& other) noexcept
	{
		reserve(other.size());
		for (auto const& v : other)
			push_back(v);
	}

	vector& operator=(vector&& other) noexcept
	{

	}
	vector& operator=(vector const& other) noexcept;

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
			m_allocator.destruct(*(m_memory + i));

		m_allocator.deallocate(m_memory);
		m_memory = nullptr;
		m_size = 0;
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
		m_allocator.destruct(*(m_memory + pos - begin()));
		move_elements(pos + 1, -1);
		--m_size;

		return iterator{ m_memory + pos };
	}

	iterator erase(const_iterator first, const_iterator last) noexcept
	{
		auto const erase_size = last - first;
		for(auto i = )
	}

	void push_back() noexcept;
	void pop_back() noexcept;

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
	}

	void realloc(std::uint64_t n) noexcept
	{
		auto* tmp_buffer = m_allocator.allocate(n * sizeof(T));
		m_allocator.align(tmp_buffer, n);

		// move current content to new buffer
		for (auto i = std::uint64_t{}; i < size(); ++i)
			m_allocator.construct(*(tmp_buffer + i), *(m_memory + i));

		m_allocator.deallocate(m_memory, m_capacity);
		m_memory = tmp_buffer;
		m_capacity = n;
	}

private:
	TAlloc m_allocator;
	std::uint64_t m_capacity;
	T *m_memory;
	std::uint64_t m_size;
};



}