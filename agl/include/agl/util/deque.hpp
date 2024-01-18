#pragma once
#include "agl/util/vector.hpp"

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
	explicit free_spaces(spaces_alloc_t const& allocator = {}) noexcept
		: m_spaces{ allocator }
	{
	}
	void push(difference_type index, difference_type size = 1) noexcept
	{
		constexpr auto const comp = [](difference_type index, space const& s) { return index < s.index; };
		auto merge_it = std::upper_bound(m_spaces.crbegin(), m_spaces.crend(), index + size, comp);
		auto forward_it = m_spaces.cbegin();

		while (merge_it != m_spaces.crend() && index == merge_it->index)
		{
			size += merge_it->size;
			forward_it = m_spaces.cbegin() + std::distance(merge_it, m_spaces.crend());
			forward_it = m_spaces.erase(forward_it);
			merge_it = m_spaces.crbegin() + std::distance(m_spaces.cbegin(), forward_it);
			merge_it = std::upper_bound(merge_it, m_spaces.crend(), index + size, comp);
		}
		merge_it = std::upper_bound(m_spaces.crbegin(), m_spaces.crend(), index, comp);
		forward_it = m_spaces.cbegin() + std::distance(merge_it, m_spaces.crend());
		m_spaces.insert(forward_it, space{ static_cast<std::uint32_t>(index), static_cast<std::uint32_t>(size) });
	}
	difference_type pop() noexcept
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
	void clear() noexcept
	{
		m_spaces.clear();
	}
private:
	spaces_t m_spaces;
};
// TODO: free_indexes must not store every index, but rather ranges of them. this will decrease amount of memory needed to track free spaces. 
template <typename T, typename TAlloc>
class block
{
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
	block() noexcept = default;
	block(std::uint64_t block_size, allocator_type const& allocator) noexcept
		: m_allocator{ allocator }
		, m_block_size{ block_size }
		, m_memory{ nullptr }
		, m_free_spaces{ free_spaces_alloc_t{ allocator } }
		, m_size{ 0 }
	{
	}
	block(block&& other) noexcept
		: m_allocator{ other.m_allocator }
		, m_block_size{ other.m_block_size }
		, m_memory{ other.m_memory }
		, m_free_spaces{ std::move(other.m_free_spaces) }
		, m_size{ other.m_size }
	{
		other.m_memory = nullptr;
	}
	block(block const& other) noexcept
		: m_allocator{ other.m_allocator }
		, m_block_size{ other.m_block_size }
		, m_memory{ nullptr }
		, m_free_spaces{ other.m_free_spaces }
		, m_size{ other.m_size }
	{
		reserve(block_size());
		for (auto i = 0; i < block_size(); ++i)
			*(m_memory + i) = *(other.m_memory + i);
	}
	block& operator=(block&& other) noexcept
	{
		if (this == &other)
			return *this;

		clear();

		m_allocator = other.m_allocator;
		m_block_size = other.m_block_size;
		m_free_spaces = std::move(other.m_free_spaces);
		m_memory = other.m_memory;
		m_size = other.m_size;
		other.m_memory = nullptr;

		return *this;
	}
	block& operator=(block const& other) noexcept
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
			*(m_memory + i) = *(other.m_memory + i);
		return *this;

	}
	~block() noexcept
	{
		clear();
	}
	const_pointer cbegin() const noexcept
	{
		AGL_ASSERT(m_memory != nullptr, "Invalid memory pointer");

		return m_memory;
	}
	const_pointer cend() const noexcept
	{
		AGL_ASSERT(m_memory != nullptr, "Invalid memory pointer");

		return m_memory + block_size();
	}
	void clear() noexcept
	{
		if (m_memory == nullptr)
			return;

		for (auto i = 0; i < block_size(); ++i)
			m_allocator.destruct(m_memory + i);

		m_allocator.deallocate(m_memory);
		m_memory = nullptr;
		m_free_spaces.clear();
		m_size = 0;
	}
	bool empty() const noexcept
	{
		return m_size == 0;
	}
	void erase(const_iterator pos) noexcept
	{
		AGL_ASSERT(cbegin() <= pos && pos < cend(), "Iterator out of bounds");

		--m_size;
		auto const index = pos - cbegin();
		m_allocator.destruct(m_memory + index);
		m_allocator.construct(m_memory + index);
		m_free_spaces.push(index);
	}
	void erase(const_iterator first, const_iterator last) noexcept
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
	void reserve(size_type n) noexcept
	{
		if (m_memory != nullptr)
			clear();

		m_block_size = n;
		m_memory = m_allocator.allocate(block_size());
		for (auto i = difference_type{}; i < static_cast<difference_type>(block_size()); ++i)
			m_allocator.construct(m_memory + i);
		
		m_free_spaces.push(0, n);
	}
	pointer push_back(value_type&& value) noexcept
	{
		if (empty())
			reserve(block_size());

		auto const index = m_free_spaces.pop();
		*(m_memory + index) = std::move(value);
		++m_size;

		return m_memory + index;
	}
	pointer push_back(value_type const& value) noexcept
	{
		if (empty())
			reserve(block_size());

		auto const index = m_free_spaces.pop();
		*(m_memory + index) = value;
		++m_size;

		return m_memory + index;
	}
	bool full() const noexcept
	{
		return size() == block_size();
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
		AGL_ASSERT(index < block_size(), "Index out of bounds");

		return *(m_memory + index);
	}
	const_reference operator[](size_type index) const noexcept
	{
		AGL_ASSERT(index < block_size(), "Index out of bounds");

		return *(m_memory + index);
	}

private:
	using free_spaces_alloc_t = typename allocator_type::template rebind<space>;
	using free_spaces_t = free_spaces<free_spaces_alloc_t>;

private:
	allocator_type m_allocator;
	T* m_memory;
	free_spaces_t m_free_spaces;
	size_type m_size;
	size_type m_block_size;
};
template <typename T>
class deque_iterator;
template <typename T>
class deque_const_iterator;
}
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

	using iterator = typename impl::deque_iterator<T>;
	using const_iterator = typename impl::deque_const_iterator<T>;
	using reverse_iterator = typename std::reverse_iterator<impl::deque_iterator<T>>;
	using reverse_const_iterator = typename std::reverse_iterator<impl::deque_const_iterator<T>>;

public:
	deque(size_type block_size = 1024, allocator_type const& allocator = {}) noexcept
		: m_blocks{ block_allocator{ allocator } }
		, m_indexes{ index_allocator{ allocator } }
		, m_block_size{ block_size }
	{
	}
	deque(deque&& other) noexcept
		: m_blocks{ std::move(other.m_blocks) }
		, m_indexes{ std::move(other.m_indexes) }
		, m_block_size{ other.m_block_size }
	{
	}
	deque(deque const& other) noexcept
		: m_blocks{ other.m_blocks }
		, m_indexes{ other.m_indexes }
		, m_block_size{ other.m_block_size }
	{
	}
	deque& operator=(deque&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_blocks = std::move(other.m_blocks);
		m_indexes = std::move(other.m_indexes);
		m_block_size = other.m_block_size;
		return *this;
	}
	deque& operator=(deque const& other) noexcept
	{
		if (this == &other)
			return *this;

		m_blocks = other.m_blocks;
		m_indexes = other.m_indexes;
		m_block_size = other.m_block_size;
		return *this;
	}
	~deque() noexcept
	{
		clear();
	}
	allocator_type get_allocator() const noexcept
	{
		return m_blocks.get_allocator();
	}
	void clear() noexcept
	{
		m_blocks.clear();
		m_indexes.clear();
	}
	reference back() noexcept
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return *m_indexes.back();
	}
	const_reference back() const noexcept
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return *m_indexes.back();
	}
	reference front() noexcept
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return at(0);
	}
	const_reference front() const noexcept
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return at(0);
	}
	bool empty() const noexcept
	{
		return size() == 0;
	}
	void erase(const_iterator pos) noexcept
	{
		AGL_ASSERT(m_indexes.cbegin() <= pos.m_it && pos.m_it < m_indexes.cend(), "Index out of bounds");
		
		auto it = find_block(pos);
		auto const index = &(*pos) - it->cbegin();
		
		it->erase(&(*pos));
		if (it->empty())
			m_blocks.erase(it);

		m_indexes.erase(pos.m_it);
	}
	/*
	void erase(const_iterator first, const_iterator last) noexcept
	{
		AGL_ASSERT(cbegin() <= first && first <= cend(), "Index out of bounds");
		AGL_ASSERT(cbegin() <= last && last <= cend(), "Index out of bounds");

		for (auto it = first; it != last; ++it)
		{
			auto it_block = find_block(pos);
			it_block->erase(pos);

			if (it_block->empty())
				m_blocks.erase(it_block);
		}
	}
	*/
	void push_back(value_type&& value) noexcept
	{
		for (auto& block : m_blocks)
			if (!block.full())
			{
				auto* ptr = block.push_back(std::move(value));
				m_indexes.push_back(ptr);
				return;
			}

		m_blocks.push_back(impl::block<T, allocator_type>{ block_size(), get_allocator() });
		auto* ptr = m_blocks.back().push_back(std::move(value));
		m_indexes.push_back(ptr);
	}
	void push_back(value_type const& value) noexcept
	{
		for (auto& block : m_blocks)
			if (!block.full())
			{
				auto* ptr = block.push_back(value);
				m_indexes.push_back(ptr);
				return;
			}

		m_blocks.push_back(impl::block<T, allocator_type>{ block_size(), get_allocator() });
		auto* ptr = m_blocks.back().push_back(value);
		m_indexes.push_back(ptr);
	}
	iterator begin() const noexcept
	{
		return iterator{ m_indexes.begin() };
	}
	const_iterator cbegin() const noexcept
	{
		return const_iterator{ m_indexes.cbegin() };
	}
	iterator end() const noexcept
	{
		return iterator{ m_indexes.end() };
	}
	const_iterator cend() const noexcept
	{
		return const_iterator{ m_indexes.cend() };
	}
	reverse_iterator rbegin() const noexcept
	{
		return reverse_iterator{ m_indexes.rbegin() };
	}
	reverse_const_iterator crbegin() const noexcept
	{
		return reverse_const_iterator{ m_indexes.crbegin() };
	}
	reverse_iterator rend() const noexcept
	{
		return reverse_iterator{ m_indexes.rend() };
	}
	reverse_const_iterator crend() const noexcept
	{
		return reverse_const_iterator{ m_indexes.crend() };
	}
	size_type size() const noexcept
	{
		return m_indexes.size();
	}
	size_type block_size() const noexcept
	{
		return m_block_size;
	}
	reference at(size_type index) noexcept
	{
		AGL_ASSERT(index < size(), "Index out of bounds");

		return *m_indexes[index];
	}
	const_reference at(size_type index) const noexcept
	{
		AGL_ASSERT(index < size(), "Index out of bounds");

		return *m_indexes[index];
	}
	reference operator[](size_type index) noexcept
	{
		AGL_ASSERT(index < size(), "Index out of bounds");

		return *m_indexes[index];
	}
	const_reference operator[](size_type index) const noexcept
	{
		AGL_ASSERT(index < size(), "Index out of bounds");

		return *m_indexes[index];
	}

private:
	using block_allocator = typename allocator_type::template rebind<impl::block<T, allocator_type>>;
	using block_vector = vector<impl::block<T, allocator_type>, block_allocator>;
	using index_allocator = typename allocator_type::template rebind<T*>;
	using index_vector = vector<T*, index_allocator>;

private:
	/// <summary>
	/// Finds a block that contains item residing under index 'value_index'.
	/// </summary>
	/// <param name="value_index"></param>
	/// <returns></returns>
	typename block_vector::iterator find_block(const_iterator pos) const noexcept
	{
		for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it)
			if (it->cbegin() <= &(*pos) && &(*pos) <= it->cend())
				return it;

		AGL_ASSERT(false, "Index out of range");
		return m_blocks.end();
	}

private:
	block_vector m_blocks;
	size_type m_block_size;
	index_vector m_indexes;
};
namespace impl
{
template <typename T>
class deque_iterator
{
public:
	using iterator = vector_iterator<T*>;
	using traits = std::iterator_traits<deque_iterator<T>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

	deque_iterator() noexcept
		: m_it{ nullptr }
	{}
	deque_iterator(iterator it) noexcept
		: m_it{ it }
	{}
	deque_iterator(deque_iterator&& other) noexcept = default;
	deque_iterator(deque_iterator const& other) noexcept = default;
	deque_iterator& operator=(deque_iterator&& other) noexcept = default;
	deque_iterator& operator=(deque_iterator const& other) noexcept = default;
	~deque_iterator() noexcept = default;
	deque_iterator& operator++() noexcept
	{
		m_it += 1;
		return *this;
	}
	deque_iterator operator++(int) const noexcept
	{
		auto result = deque_iterator{ *this };
		return ++result;
	}
	deque_iterator operator+(difference_type offset) const noexcept
	{
		return deque_iterator{ m_it + offset };
	}
	deque_iterator& operator+=(difference_type offset) noexcept
	{
		m_it += offset;
		return *this;
	}
	deque_iterator& operator--() noexcept
	{
		m_it -= 1;
		return *this;
	}
	deque_iterator operator--(int) const noexcept
	{
		auto result = deque_iterator{ *this };
		return --result;
	}
	difference_type operator-(deque_iterator rhs) const noexcept
	{
		return m_it - rhs.m_it;
	}
	deque_iterator operator-(difference_type offset) const noexcept
	{
		return deque_iterator{ m_it - offset };
	}
	deque_iterator& operator-=(difference_type offset) noexcept
	{
		m_it -= offset;
		return *this;
	}
	reference operator*() const noexcept
	{
		return *(*m_it);
	}
	pointer operator->() const noexcept
	{
		return *m_it;
	}

private:
	template <typename U>
	friend class deque_const_iterator;

	template <typename U>
	friend bool operator==(deque_iterator<U> const& lhs, deque_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator!=(deque_iterator<U> const& lhs, deque_iterator<U> const& rhs) noexcept;

private:
	iterator m_it;
};
template <typename T>
bool operator==(deque_iterator<T> const& lhs, deque_iterator<T> const& rhs) noexcept
{
	return lhs.m_it == rhs.m_it;
}
template <typename T>
bool operator!=(deque_iterator<T> const& lhs, deque_iterator<T> const& rhs) noexcept
{
	return lhs.m_it != rhs.m_it;
}
template <typename T>
class deque_const_iterator
{
public:
	using iterator = vector_const_iterator<T*>;
	using traits = std::iterator_traits<deque_const_iterator<T>>;
	using value_type = typename traits::value_type;
	using pointer = typename traits::pointer;
	using reference = typename traits::reference;
	using difference_type = typename traits::difference_type;

public:
	deque_const_iterator() noexcept
		: m_it{ nullptr }
		, m_size{ 0 }
	{}
	deque_const_iterator(iterator it) noexcept
		: m_it{ it }
	{}
	deque_const_iterator(deque_iterator<T> const& other) noexcept
		: m_it{ other.m_it }
	{
	}
	deque_const_iterator(deque_iterator<T>&& other) noexcept
		: m_it{ other.m_it }
	{
	}
	deque_const_iterator(deque_const_iterator&& other) noexcept = default;
	deque_const_iterator(deque_const_iterator const& other) noexcept = default;
	deque_const_iterator& operator=(deque_const_iterator&& other) noexcept = default;
	deque_const_iterator& operator=(deque_const_iterator const& other) noexcept = default;
	~deque_const_iterator() noexcept = default;
	deque_const_iterator& operator++() noexcept
	{
		m_it += 1;
		return *this;
	}
	deque_const_iterator operator++(int) const noexcept
	{
		auto result = deque_iterator{ *this };
		return ++result;
	}
	deque_const_iterator operator+(difference_type offset) const noexcept
	{
		return deque_const_iterator{ m_it + offset };
	}
	deque_const_iterator& operator+=(difference_type offset) noexcept
	{
		m_it += offset;
		return *this;
	}
	deque_const_iterator& operator--() noexcept
	{
		m_it -= 1;
		return *this;
	}
	deque_const_iterator operator--(int) const noexcept
	{
		auto result = deque_iterator{ *this };
		return --result;
	}
	difference_type operator-(deque_const_iterator rhs) const noexcept
	{
		return m_it - rhs.m_it;
	}
	deque_const_iterator operator-(difference_type offset) const noexcept
	{
		return deque_const_iterator{ m_it - offset };
	}
	deque_const_iterator& operator-=(difference_type offset) noexcept
	{
		m_it -= offset;
		return *this;
	}
	reference operator*() const noexcept
	{
		return *(*m_it);
	}
	pointer operator->() const noexcept
	{
		return *m_it;
	}

private:
	template <typename U, typename W>
	friend class deque;

	template <typename U>
	friend bool operator==(deque_const_iterator<U> const& lhs, deque_const_iterator<U> const& rhs) noexcept;

	template <typename U>
	friend bool operator!=(deque_const_iterator<U> const& lhs, deque_const_iterator<U> const& rhs) noexcept;

private:
	iterator m_it;
};
template <typename T>
bool operator==(deque_const_iterator<T> const& lhs, deque_const_iterator<T> const& rhs) noexcept
{
	return lhs.m_it == rhs.m_it;
}
template <typename T>
bool operator!=(deque_const_iterator<T> const& lhs, deque_const_iterator<T> const& rhs) noexcept
{
	return lhs.m_it != rhs.m_it;
}
}
}
namespace std
{
template <typename T>
struct iterator_traits<agl::impl::deque_iterator<T>>
{
	using value_type = typename ::agl::type_traits<T>::value_type;
	using pointer = typename ::agl::type_traits<T>::pointer;
	using reference = typename ::agl::type_traits<T>::reference;
	using difference_type = typename ::agl::type_traits<T>::difference_type;
	using iterator_category = random_access_iterator_tag;

};
template <typename T>
struct iterator_traits<agl::impl::deque_const_iterator<T>>
{
	using value_type = typename ::agl::type_traits<T>::value_type;
	using pointer = typename ::agl::type_traits<T>::pointer;
	using reference = typename ::agl::type_traits<T>::reference;
	using difference_type = typename ::agl::type_traits<T>::difference_type;
	using iterator_category = random_access_iterator_tag;
};
/*
template <typename T>
struct iterator_traits<agl::impl::deque_iterator<T*>>
{
	using value_type = T*;
	using pointer = T**;
	using reference = T*;
	using difference_type = ptrdiff_t;
	using iterator_category = random_access_iterator_tag;
};
template <typename T>
struct iterator_traits<agl::impl::deque_const_iterator<T*>>
{
	using value_type = T const*;
	using pointer = T const**;
	using reference = T const*;
	using difference_type = ptrdiff_t;;
	using iterator_category = random_access_iterator_tag;
};
*/
}