/*#pragma once
#include "agl/memory/allocator.hpp"

namespace agl
{
template <typename T, typename TAllocator = mem::allocator<T>>
class list
{
public:
	struct node
	{
		T data;
		node* next;
	};

public:
	using allocator_type = typename TAllocator::template rebind<node>;
	using value_type = typename type_traits<T>::value_type;
	using pointer = typename type_traits<T>::pointer;
	using const_pointer = typename type_traits<T>::const_pointer;
	using reference = typename type_traits<T>::reference;
	using const_reference = typename type_traits<T>::const_reference;
	using size_type = typename type_traits<T>::size_type;
	using difference_type = typename type_traits<T>::difference_type;

	using iterator = void;
	using const_iterator = void;
	using reverse_iterator = void;
	using reverse_const_iterator = void;

public:
	list()
		: m_capacity{ 0 }
		, m_head{ nullptr }
		, m_tail{ nullptr }
		, m_size{ 0 }
	{
	}
	explicit list(allocator_type const& allocator)
		: list{}
		, m_allocator{ allocator }
	{
	}
	list(list&& other)
		: m_allocator{ std::move(other.m_allocator) }
		, m_capacity{ other.m_capacity }
		, m_head{ other.m_head }
		, m_tail{ other.m_tail }
		, m_size{ other.m_size }
	{
		other.m_head = nullptr;
		other.m_tail = nullptr;
	}
	list(list const& other)
	{
		reserve(other.capacity());
		for (auto const& v : other)
			emplace_back(v);
	}
	list& operator=(list&& other)
	{
		if (this == &other)
			return *this;

		m_allocator = std::move(other.m_allocator);
		m_capacity = other.m_capacity;
		m_head = other.m_head;
		other.m_head = nullptr;
		m_tail = other.m_tail;
		other.m_tail = nullptr;
		m_size = other.m_size;
		return *this;
	}
	list& operator=(list const& other)
	{
		if (this == &other)
			return *this;

		m_allocator = other.m_allocator;
		m_capacity = other.m_capacity;
		m_head = other.m_head;
		other.m_head = nullptr;
		m_tail = other.m_tail;
		other.m_tail = nullptr;
		m_size = other.m_size;
		return *this;
	}
	~list()
	{
		clear();
	}

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	iterator rbegin();
	iterator rend();
	const_iterator rbegin() const;
	const_iterator rend() const;
	const_iterator crbegin() const;
	const_iterator crend() const;

	void clear();
	std::uint64_t capacity() const
	{
		return m_capacity;
	}
	iterator insert(const_iterator pos, value_type&& value);
	iterator insert(const_iterator pos, const_reference value);
	iterator emplace(const_iterator pos, value_type&& value);
	iterator emplace(const_iterator pos, value_type&& value);
	iterator emplace_back(const_iterator pos, const_reference value);
	iterator emplace_back(const_iterator pos, const_reference value);
	iterator emplace_front(const_iterator pos, const_reference value);
	iterator emplace_front(const_iterator pos, const_reference value);
	iterator erase(const_iterator pos);
	iterator erase(const_iterator first, const_iterator last);
	void push_back(value_type&& value);
	void push_back(const_reference value);
	void pop_back();
	void push_front();
	void resize();
	void reserve();
	std::uint64_t size() const;

	T& at();
	T const& at() const;

private:
	allocator_type m_allocator;
	std::uint64_t m_capacity;
	node* m_head;
	node* m_tail;
	std::uint64_t m_size;
};
}*/