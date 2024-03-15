#pragma once
#include "agl/vector.hpp"

namespace agl
{
namespace impl
{
struct space
{
	std::uint32_t index; // begin
	std::uint32_t size;  // how many after
};
template <typename TAlloc>
class free_spaces
{
public:
	using spaces_alloc_t = typename TAlloc::template rebind<space>;
	using spaces_t = vector<space, spaces_alloc_t>;
	using difference_type = typename spaces_t::difference_type;

public:
	explicit free_spaces(spaces_alloc_t const& allocator = {})
		: m_spaces{ allocator }
	{
	}
	void push(difference_type index, difference_type size = 1)
	{
		constexpr auto const comp = [](space const& s, difference_type index) { return s.index >= index; };

		// check if any space overlaps with this and merge it
		// TODO: maybe it might be necessary to find just the first occurrence of the overlap

		// check for overlap
		auto it = std::lower_bound(m_spaces.begin(), m_spaces.end(), index + size, comp);
		if (it != m_spaces.end())
		{
			it->size += static_cast<std::uint32_t>(size);
			it->index -= static_cast<std::uint32_t>(size);
		}
		else
			m_spaces.insert(it, space{ static_cast<std::uint32_t>(index), static_cast<std::uint32_t>(size) });
	}
	difference_type pop()
	{
		auto& back = m_spaces.back();
		auto index = back.index;
		
		if (back.size > 1)
		{
			++back.index;
			--back.size;
		}
		else
			m_spaces.erase(m_spaces.cend() - 1);
		
		return index;
	}
	void clear()
	{
		m_spaces.clear();
	}
private:
	spaces_t m_spaces;
};
// TODO: free_indexes must not store every index, but rather ranges of them. 
// this decreases amount of memory needed to track free spaces but increases complexity of the alg.
template <typename T, typename TAlloc>
class block
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

	using iterator = typename type_traits<T>::pointer;
	using const_iterator = typename type_traits<T>::const_pointer;

public:
	block()
		: m_memory{ nullptr }
		, m_size{ 0 }
	{
	}
	explicit block(std::uint64_t block_size, allocator_type const& allocator)
		: m_allocator{ allocator }
		, m_block_size{ block_size }
		, m_memory{ nullptr }
		, m_free_spaces{ free_spaces_alloc_t{ allocator } }
		, m_size{ 0 }
	{
	}
	block(block&& other)
		: m_allocator{ std::move(other.m_allocator) }
		, m_block_size{ other.m_block_size }
		, m_memory{ other.m_memory }
		, m_free_spaces{ std::move(other.m_free_spaces) }
		, m_size{ other.m_size }
	{
		other.m_memory = nullptr;
	}
	block(block const& other)
		: m_allocator{ other.m_allocator }
		, m_block_size{ other.m_block_size }
		, m_memory{ nullptr }
		, m_free_spaces{ other.m_free_spaces }
		, m_size{ other.m_size }
	{
		reserve(block_size());
		for (auto i = 0; i < block_size(); ++i)
			make_copy(m_memory + i, *(other.m_memory + i));
	}
	block& operator=(block&& other)
	{
		if (this == &other)
			return *this;

		clear();

		m_allocator = std::move(other.m_allocator);
		m_block_size = other.m_block_size;
		m_free_spaces = std::move(other.m_free_spaces);
		m_memory = other.m_memory;
		m_size = other.m_size;
		other.m_memory = nullptr;
		return *this;
	}
	block& operator=(block const& other)
	{
		if (this == &other)
			return *this;

		clear();

		m_allocator = other.m_allocator;
		m_block_size = other.m_block_size;
		m_free_spaces = other.m_free_spaces;
		m_size = other.m_size;
		
		reserve(block_size());
		for (auto i = 0; i < block_size(); ++i)
			make_copy(m_memory + i, *(other.m_memory + i));

		return *this;
	}
	~block()
	{
		clear();
	}
	const_pointer cbegin() const
	{
		AGL_ASSERT(m_memory != nullptr, "Invalid memory pointer");

		return m_memory;
	}
	const_pointer cend() const
	{
		AGL_ASSERT(m_memory != nullptr, "Invalid memory pointer");

		return m_memory + block_size();
	}
	bool pointer_in_bounds(const_pointer ptr) const
	{
		return m_memory != nullptr
			&& m_memory <= ptr
			&& m_memory + block_size() > ptr;
	}
	void clear()
	{
		if (m_memory == nullptr)
			return;

		for (auto i = 0; i < block_size(); ++i)
			m_allocator.destruct(m_memory + i);

		m_allocator.deallocate(m_memory, block_size());
		m_memory = nullptr;
		m_free_spaces.clear();
		m_size = 0;
	}
	bool empty() const
	{
		return m_size == 0;
	}
	void erase(const_iterator pos)
	{
		AGL_ASSERT(cbegin() <= pos && pos < cend(), "Iterator out of bounds");

		auto const index = pos - cbegin();
		m_allocator.destruct(m_memory + index);
		m_allocator.construct(m_memory + index);
		m_free_spaces.push(index);
		--m_size;
	}
	void erase(const_iterator first, const_iterator last)
	{
		AGL_ASSERT(cbegin() <= first && first < cend(), "Index out of bounds");
		AGL_ASSERT(cbegin() <= last && last < cend(), "Index out of bounds");

		auto const erase_size = last - first;
		auto const index = first - cbegin();
		m_size -= erase_size;
		
		auto i = 0;
		for (auto it = first; it != last; ++it, ++i)
		{
			m_allocator.destruct(m_memory + index + i);
			m_allocator.construct(m_memory + index + i);
			m_free_spaces.push(index + i);
		}
	}
	void reserve(size_type n)
	{
		if (m_memory != nullptr)
			clear();

		m_block_size = n;
		m_memory = m_allocator.allocate(block_size());
		for (auto i = difference_type{}; i < static_cast<difference_type>(block_size()); ++i)
			m_allocator.construct(m_memory + i);
		
		m_free_spaces.push(0, n);
	}
	pointer push_back(value_type&& value)
	{
		AGL_ASSERT(!full(), "index out of bounds");

		if (empty())
			reserve(block_size());

		auto const index = m_free_spaces.pop();
		make_move(m_memory + index, std::forward<value_type&&>(value));
		++m_size;

		return m_memory + index;
	}
	pointer push_back(const_reference value)
	{
		AGL_ASSERT(!full(), "index out of bounds");

		if (empty())
			reserve(block_size());

		auto const index = m_free_spaces.pop();
		make_copy(m_memory + index, value);
		++m_size;

		return m_memory + index;
	}
	template <typename... TArgs>
	pointer emplace_back(TArgs&&... args)
	{
		AGL_ASSERT(!full(), "index out of bounds");

		if (empty()) // you can do better by delegating block creation to deque
			reserve(block_size());
		
		auto const index = m_free_spaces.pop();
		m_allocator.destruct(m_memory + index);
		m_allocator.construct(m_memory + index, std::forward<TArgs>(args)...);
		++m_size;

		return m_memory + index;
	}
	bool full() const
	{
		return size() == block_size();
	}
	size_type size() const
	{
		return m_size;
	}
	size_type block_size() const
	{
		return m_block_size;
	}
	reference operator[](size_type index)
	{
		AGL_ASSERT(!full(), "index out of bounds");

		return *(m_memory + index);
	}
	const_reference operator[](size_type index) const
	{
		AGL_ASSERT(index < block_size(), "Index out of bounds");

		return *(m_memory + index);
	}

private:
	void make_copy(pointer dest, const_reference src)
	{
		if constexpr (std::is_copy_constructible_v<value_type>)
			m_allocator.construct(dest, src);
		else if constexpr (std::is_copy_assignable_v<value_type>)
			*dest = src;
		else
			static_assert(false, "invalid use of copy function - type is not copyable");
	}
	void make_move(pointer dest, value_type&& src)
	{
		if constexpr (std::is_move_constructible_v<value_type>)
			m_allocator.construct(dest, std::forward<value_type&&>(src));
		else if constexpr (std::is_move_assignable_v<value_type>)
			*dest = std::move(src);
		else
			static_assert(false, "invalid use of copy function - type is not movalbe");
	}

private:
	using free_spaces_alloc_t = typename allocator_type::template rebind<space>;
	using free_spaces_t = free_spaces<free_spaces_alloc_t>;

private:
	allocator_type m_allocator;
	size_type m_block_size;
	free_spaces_t m_free_spaces;
	T* m_memory;
	size_type m_size;
};
}

template <typename T>
class deque_iterator;

template <typename T>
class deque_const_iterator;

template <typename T>
class deque_reverse_iterator;

template <typename T>
class deque_reverse_const_iterator;


/**
 * @brief 
 * Shares properties with std::deque, with the difference that the size of a block is customizable on the creation of the data structure.
 * @tparam T 
 * @tparam TAlloc 
 */
template <typename T, typename TAlloc = agl::mem::allocator<T>>
class deque
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

private:
	using block = typename impl::block<T, allocator_type>;
	using block_allocator = typename allocator_type::template rebind<block>;
	using block_vector = vector<block, block_allocator>;
	using index_allocator = typename allocator_type::template rebind<T*>;
	using index_vector = vector<T*, index_allocator>;

public:
	using iterator = typename deque_iterator<typename index_vector::iterator>;
	using const_iterator = typename deque_const_iterator<typename index_vector::const_iterator>;
	using reverse_iterator = typename deque_iterator<typename index_vector::reverse_iterator>;
	using reverse_const_iterator = typename deque_const_iterator<typename index_vector::reverse_const_iterator>;

public:
	deque(size_type block_size = 1024, allocator_type const& allocator = {})
		: m_blocks{ block_allocator{ allocator } }
		, m_indexes{ index_allocator{ allocator } }
		, m_block_size{ block_size }
	{
	}
	deque(deque&& other)
		: m_blocks{ std::move(other.m_blocks) }
		, m_indexes{ std::move(other.m_indexes) }
		, m_block_size{ other.m_block_size }
	{
	}
	deque(deque const& other)
		: m_blocks{ other.m_blocks }
		, m_indexes{ other.m_indexes }
		, m_block_size{ other.m_block_size }
	{
	}
	deque& operator=(deque&& other)
	{
		if (this == &other)
			return *this;

		m_blocks = std::move(other.m_blocks);
		m_indexes = std::move(other.m_indexes);
		m_block_size = other.m_block_size;
		return *this;
	}
	deque& operator=(deque const& other)
	{
		if (this == &other)
			return *this;

		m_blocks = other.m_blocks;
		m_indexes = other.m_indexes;
		m_block_size = other.m_block_size;
		return *this;
	}
	~deque()
	{
		clear();
	}
	allocator_type get_allocator() const
	{
		return m_blocks.get_allocator();
	}
	void clear()
	{
		m_blocks.clear();
		m_indexes.clear();
	}
	reference back()
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return *m_indexes.back();
	}
	const_reference back() const
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return *m_indexes.back();
	}
	reference front()
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return at(0);
	}
	const_reference front() const
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return at(0);
	}
	bool empty() const
	{
		return size() == 0;
	}
	void erase(const_pointer ptr)
	{
		auto it = find_block(ptr);
		it->erase(ptr);

	}
	void erase(const_iterator pos)
	{
		AGL_ASSERT(m_indexes.cbegin() <= pos.m_it && pos.m_it < m_indexes.cend(), "Index out of bounds");
		
		auto it = find_block(&(*pos));
		auto const index = &(*pos) - it->cbegin();
		
		it->erase(&(*pos));
		if (it->empty())
			m_blocks.erase(it);

		m_indexes.erase(pos.m_it);
	}
	void erase(const_iterator first, const_iterator last)
	{
		AGL_ASSERT(cbegin() <= first && first <= cend(), "Index out of bounds");
		AGL_ASSERT(cbegin() <= last && last <= cend(), "Index out of bounds");

		for (auto it = first; it != last; ++it)
		{
			auto it_block = find_block(&(*pos));
			it_block->erase(pos);

			if (it_block->empty())
				m_blocks.erase(it_block);
		}
	}
	template <typename... TArgs>
	iterator emplace(const_iterator pos, TArgs... args)
	{
		auto* ptr = find_and_emplace(std::forward<TArgs>(args)...);
		return m_indexes.insert(pos.m_it, ptr);
	}
	template <typename... TArgs>
	iterator emplace_back(TArgs... args)
	{
		auto* ptr = find_and_emplace(std::forward<TArgs>(args)...);
		return m_indexes.insert(m_indexes.cend(), ptr);
	}
	iterator insert(const_iterator pos, value_type&& value)
	{
		auto* ptr = find_and_push(std::move(value));
		return m_indexes.insert(pos.m_it, ptr);
	}
	iterator insert(const_iterator pos, const_reference value)
	{
		auto* ptr = find_and_push(value);
		return m_indexes.insert(pos.m_it, ptr);
	}
	void push_back(value_type&& value)
	{
		auto* ptr = find_and_push(std::move(value));
		m_indexes.push_back(ptr);
	}
	void push_back(const_reference value)
	{
		auto* ptr = find_and_push(value);
		m_indexes.push_back(ptr);
	}
	iterator begin()
	{
		return iterator{ m_indexes.begin() };
	}
	const_iterator begin() const
	{
		return iterator{ m_indexes.cbegin() };
	}
	const_iterator cbegin() const
	{
		return const_iterator{ m_indexes.cbegin() };
	}
	iterator end()
	{
		return iterator{ m_indexes.end() };
	}
	const_iterator end() const
	{
		return iterator{ m_indexes.cend() };
	}
	const_iterator cend() const
	{
		return const_iterator{ m_indexes.cend() };
	}
	reverse_iterator rbegin()
	{
		return reverse_iterator{ m_indexes.rbegin() };
	}
	reverse_const_iterator rbegin() const
	{
		return reverse_iterator{ m_indexes.crbegin() };
	}
	reverse_const_iterator crbegin() const
	{
		return reverse_const_iterator{ m_indexes.crbegin() };
	}
	reverse_iterator rend()
	{
		return reverse_iterator{ m_indexes.rend() };
	}
	reverse_const_iterator rend() const
	{
		return reverse_iterator{ m_indexes.crend() };
	}
	reverse_const_iterator crend() const
	{
		return reverse_const_iterator{ m_indexes.crend() };
	}
	size_type size() const
	{
		return m_indexes.size();
	}
	size_type block_size() const
	{
		return m_block_size;
	}
	reference at(size_type index)
	{
		AGL_ASSERT(index < size(), "Index out of bounds");

		return *m_indexes[index];
	}
	const_reference at(size_type index) const
	{
		AGL_ASSERT(index < size(), "Index out of bounds");

		return *m_indexes[index];
	}
	reference operator[](size_type index)
	{
		AGL_ASSERT(index < size(), "Index out of bounds");

		return *m_indexes[index];
	}
	const_reference operator[](size_type index) const
	{
		AGL_ASSERT(index < size(), "Index out of bounds");

		return *m_indexes[index];
	}

private:
	/// <summary>
	/// Finds a block that contains item residing under index 'value_index'.
	/// </summary>
	/// <param name="value_index"></param>
	/// <returns></returns>
	typename block_vector::iterator find_block(const_pointer ptr)
	{
		for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
			if(it->pointer_in_bounds(ptr))
				return it;

		AGL_ASSERT(false, "Index out of range");
		return m_blocks.end();
	}
	block& free_block() // gets first free block or creates new one
	{
		for (auto& block : m_blocks)
			if (!block.full())
				return block;
	
		auto allocator = typename block::allocator_type{ get_allocator() };
		return *m_blocks.emplace_back(block_size(), allocator);
	}
	typename block::pointer find_and_push(const_reference value)
	{
		auto& block = free_block();
		return block.push_back(value);
	}
	typename block::pointer find_and_push(value_type&& value)
	{
		auto& block = free_block();
		return block.push_back(std::move(value));
	}
	template <typename... TArgs>
	typename block::pointer find_and_emplace(TArgs&&... args)
	{
		auto& block = free_block();
		return block.emplace_back(std::forward<TArgs>(args)...);
	}

private:
	block_vector m_blocks;
	size_type m_block_size;
	index_vector m_indexes;
};

template <typename T>
class deque_iterator
{
public:
	using iterator = T;
	using traits = std::iterator_traits<deque_iterator<T>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	deque_iterator()
		: m_it{ nullptr }
	{
	}
	deque_iterator(iterator it)
		: m_it{ it }
	{
	}
	deque_iterator(deque_iterator&& other)
		: m_it{ other.m_it }
	{
	}
	deque_iterator(deque_iterator const& other)
		: m_it{ other.m_it }
	{
	}
	deque_iterator& operator=(deque_iterator&& other)
	{
		m_it = other.m_it;
		return *this;
	}
	deque_iterator& operator=(deque_iterator const& other)
	{
		m_it = other.m_it;
		return *this;
	}
	~deque_iterator()
	{
	}
	deque_iterator& operator++()
	{
		m_it += 1;
		return *this;
	}
	deque_iterator operator++(int) const
	{
		auto result = deque_iterator{ *this };
		return ++result;
	}
	deque_iterator operator+(difference_type offset) const
	{
		return deque_iterator{ m_it + offset };
	}
	deque_iterator& operator+=(difference_type offset)
	{
		m_it += offset;
		return *this;
	}
	deque_iterator& operator--()
	{
		m_it -= 1;
		return *this;
	}
	deque_iterator operator--(int) const
	{
		auto result = deque_iterator{ *this };
		return --result;
	}
	difference_type operator-(deque_iterator rhs) const
	{
		return m_it - rhs.m_it;
	}
	deque_iterator operator-(difference_type offset) const
	{
		return deque_iterator{ m_it - offset };
	}
	deque_iterator& operator-=(difference_type offset)
	{
		m_it -= offset;
		return *this;
	}
	reference operator*() const
	{
		return *(*m_it);
	}
	pointer operator->() const
	{
		return *m_it;
	}

private:
	template <typename U>
	friend class deque_const_iterator;

	template <typename U>
	friend bool operator==(deque_iterator<U> const& lhs, deque_iterator<U> const& rhs);

	template <typename U>
	friend bool operator!=(deque_iterator<U> const& lhs, deque_iterator<U> const& rhs);

	template <typename U>
	friend bool operator<(deque_iterator<U> const& lhs, deque_iterator<U> const& rhs);

	template <typename U>
	friend bool operator<=(deque_iterator<U> const& lhs, deque_iterator<U> const& rhs);

	template <typename U>
	friend bool operator>(deque_iterator<U> const& lhs, deque_iterator<U> const& rhs);

	template <typename U>
	friend bool operator>=(deque_iterator<U> const& lhs, deque_iterator<U> const& rhs);

private:
	iterator m_it;
};
template <typename T>
bool operator==(deque_iterator<T> const& lhs, deque_iterator<T> const& rhs)
{
	return lhs.m_it == rhs.m_it;
}
template <typename T>
bool operator!=(deque_iterator<T> const& lhs, deque_iterator<T> const& rhs)
{
	return lhs.m_it != rhs.m_it;
}
template <typename T>
bool operator<(deque_iterator<T> const& lhs, deque_iterator<T> const& rhs)
{
	return lhs.m_it < rhs.m_it;
}
template <typename T>
bool operator<=(deque_iterator<T> const& lhs, deque_iterator<T> const& rhs)
{
	return lhs.m_it <= rhs.m_it;
}
template <typename T>
bool operator>(deque_iterator<T> const& lhs, deque_iterator<T> const& rhs)
{
	return lhs.m_it > rhs.m_it;
}
template <typename T>
bool operator>=(deque_iterator<T> const& lhs, deque_iterator<T> const& rhs)
{
	return lhs.m_it >= rhs.m_it;
}

template <typename T>
class deque_const_iterator
{
public:
	using iterator = T;
	using traits = std::iterator_traits<deque_const_iterator<T>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

public:
	deque_const_iterator()
		: m_it{ nullptr }
		, m_size{ 0 }
	{
	}
	deque_const_iterator(iterator it)
		: m_it{ it }
	{
	}
	deque_const_iterator(deque_iterator<T> const& other)
		: m_it{ other.m_it }
	{
	}
	deque_const_iterator(deque_iterator<T>&& other)
		: m_it{ other.m_it }
	{
	}
	deque_const_iterator(deque_const_iterator&& other)
		: m_it{ other.m_it }
	{
	}
	deque_const_iterator(deque_const_iterator const& other)
		: m_it{ other.m_it }
	{
	}
	deque_const_iterator& operator=(deque_const_iterator&& other)
	{
		m_it = other.m_it;
		return *this;
	}
	deque_const_iterator& operator=(deque_const_iterator const& other)
	{
		m_it = other.m_it;
		return *this;
	}
	~deque_const_iterator()
	{
	}
	deque_const_iterator& operator++()
	{
		m_it += 1;
		return *this;
	}
	deque_const_iterator operator++(int) const
	{
		auto result = deque_iterator{ *this };
		return ++result;
	}
	deque_const_iterator operator+(difference_type offset) const
	{
		return deque_const_iterator{ m_it + offset };
	}
	deque_const_iterator& operator+=(difference_type offset)
	{
		m_it += offset;
		return *this;
	}
	deque_const_iterator& operator--()
	{
		m_it -= 1;
		return *this;
	}
	deque_const_iterator operator--(int) const
	{
		auto result = deque_iterator{ *this };
		return --result;
	}
	difference_type operator-(deque_const_iterator rhs) const
	{
		return m_it - rhs.m_it;
	}
	deque_const_iterator operator-(difference_type offset) const
	{
		return deque_const_iterator{ m_it - offset };
	}
	deque_const_iterator& operator-=(difference_type offset)
	{
		m_it -= offset;
		return *this;
	}
	reference operator*() const
	{
		return *(*m_it);
	}
	pointer operator->() const
	{
		return *m_it;
	}

private:
	template <typename U, typename W>
	friend class deque;

	template <typename U>
	friend bool operator==(deque_const_iterator<U> const& lhs, deque_const_iterator<U> const& rhs);

	template <typename U>
	friend bool operator!=(deque_const_iterator<U> const& lhs, deque_const_iterator<U> const& rhs);

	template <typename U>
	friend bool operator<(deque_const_iterator<U> const& lhs, deque_const_iterator<U> const& rhs);

	template <typename U>
	friend bool operator<=(deque_const_iterator<U> const& lhs, deque_const_iterator<U> const& rhs);

	template <typename U>
	friend bool operator>(deque_const_iterator<U> const& lhs, deque_const_iterator<U> const& rhs);

	template <typename U>
	friend bool operator>=(deque_const_iterator<U> const& lhs, deque_const_iterator<U> const& rhs);

private:
	iterator m_it;
};
template <typename T>
bool operator==(deque_const_iterator<T> const& lhs, deque_const_iterator<T> const& rhs)
{
	return lhs.m_it == rhs.m_it;
}
template <typename T>
bool operator!=(deque_const_iterator<T> const& lhs, deque_const_iterator<T> const& rhs)
{
	return lhs.m_it != rhs.m_it;
}
template <typename T>
bool operator<(deque_const_iterator<T> const& lhs, deque_const_iterator<T> const& rhs)
{
	return lhs.m_it < rhs.m_it;
}
template <typename T>
bool operator<=(deque_const_iterator<T> const& lhs, deque_const_iterator<T> const& rhs)
{
	return lhs.m_it <= rhs.m_it;
}
template <typename T>
bool operator>(deque_const_iterator<T> const& lhs, deque_const_iterator<T> const& rhs)
{
	return lhs.m_it > rhs.m_it;
}
template <typename T>
bool operator>=(deque_const_iterator<T> const& lhs, deque_const_iterator<T> const& rhs)
{
	return lhs.m_it >= rhs.m_it;
}
}

namespace std
{
template <typename T>
struct iterator_traits<agl::deque_iterator<T>>
{
	using raw_value_type = std::remove_pointer_t<typename T::value_type>;
	using traits = typename agl::type_traits<raw_value_type>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;
	using iterator_category = random_access_iterator_tag;

};

template <typename T>
struct iterator_traits<agl::deque_const_iterator<T>>
{
	using raw_value_type = std::remove_pointer_t<typename T::value_type>;
	using traits = typename agl::type_traits<raw_value_type>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;
	using iterator_category = random_access_iterator_tag;
};
}