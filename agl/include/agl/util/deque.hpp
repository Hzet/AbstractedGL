#pragma once
#include "agl/util/vector.hpp"
#include "agl/util/memory/containers.hpp"

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
		, m_size{ 0 }
		, m_block_size{ block_size }
	{
	}
	const_pointer cbegin() const noexcept
	{
		return &m_data[0];
	}
	const_pointer cend() const noexcept
	{
		return &m_data[0];
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
		m_data.erase(m_data.cbegin() + index);
		m_free_indexes.push_back(index);
	}
	void erase(difference_type first, difference_type last) noexcept
	{
		auto const erase_size = last - first;
		m_size -= erase_size;
		m_data.erase(m_data.cbegin() + first, m_data.cbegin() + last);

		for (auto i = first; i < last; ++i)
			m_free_indexes.push_back(i);

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
	pointer push_back(value_type&& value) noexcept
	{
		if (empty())
			reserve(block_size());

		auto const index = m_free_indexes.back();
		m_free_indexes.pop_back();
		m_data[index] = std::move(value);
		++m_size;

		return &m_data[index];
	}
	pointer push_back(value_type const& value) noexcept
	{
		if (empty())
			reserve(block_size());

		auto const index = m_free_indexes.back();
		m_free_indexes.pop_back();
		m_data[index] = value;
		++m_size;

		return &m_data[index];
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
		return m_data[index];
	}
	const_reference operator[](size_type index) const noexcept
	{
		return m_data[index];
	}

private:
	mem::vector<T, TAlloc> m_data;
	mem::vector<std::uint16_t, TAlloc> m_free_indexes;
	size_type m_size;
	size_type m_block_size;
};
template <typename T, typename TTraits>
class deque_iterator;
template <typename T, typename TTraits>
class deque_const_iterator;
template <typename T, typename TTraits>
class deque_reverse_iterator;
template <typename T, typename TTraits>
class deque_reverse_const_iterator;
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
	using reverse_iterator = typename impl::deque_reverse_iterator<T, self>;
	using reverse_const_iterator = typename impl::deque_reverse_const_iterator<T, self>;

public:
	deque(std::uint64_t block_size = 1024, allocator_type const& allocator = {}) noexcept
		: m_blocks{ allocator }
		, m_block_size{ block_size }
		, m_size{ 0 }
	{
	}
	allocator_type get_allocator() const noexcept
	{
		return m_blocks.get_allocator();
	}
	void clear() noexcept
	{
		for (auto& block : m_blocks)
			block.clear();
	}
	reference back() noexcept
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return at(size() - 1);
	}
	const_reference back() const noexcept
	{
		AGL_ASSERT(!empty(), "Index out of range");

		return at(size() - 1);
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
	void erase(const_iterator pos) noexcept
	{
		auto& block = find_block(pos);
		block.erase(pos);
	}
	void erase(const_iterator first, const_iterator last) noexcept
	{
		for (auto it = first; it != last; ++it)
		{
			auto& block = find_block(pos);
			block.erase(pos);
		}
	}
	void push_back(value_type&& value) noexcept
	{
		for (auto& block : m_blocks)
			if (!block.full())
			{
				auto* ptr = block.push_back(std::move(value));
				m_indexes.push_back(ptr);
			}
	}
	void push_back(value_type const& value) noexcept
	{
		for (auto& block : m_blocks)
			if (!block.full())
			{
				auto* ptr = block.push_back(value);
				m_indexes.push_back(ptr);
			}
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
		return *m_indexes[index];
	}
	const_reference at(size_type index) const noexcept
	{
		return *m_indexes[index];
	}
	reference operator[](size_type index) noexcept
	{
		return *m_indexes[index];
	}
	const_reference operator[](size_type index) const noexcept
	{
		return *m_indexes[index];
	}

private:
	/// <summary>
	/// Finds a block that contains item residing under index 'value_index'.
	/// </summary>
	/// <param name="value_index"></param>
	/// <returns></returns>
	impl::block<T, TAlloc>& find_block(const_iterator pos) const noexcept
	{
		for (auto& block : m_blocks)
			if (block.cbegin() <= &(*pos) && block.cend() <= &(*pos))
				return block;

		AGL_ASSERT(false, "Index out of range");
	}

private:
	mem::vector<impl::block<T, TAlloc>, TAlloc> m_blocks;
	size_type m_block_size;
	//vector<T*, TAlloc> m_indexes;
	mem::vector<T*> m_indexes;
};
namespace impl
{
template <typename T, typename TTraits>
class deque_reverse_iterator
{
public:
	using iterator = vector_reverse_iterator<T, TTraits>;
	using value_type = typename TTraits::value_type;
	using pointer = typename TTraits::pointer;
	using const_pointer = typename TTraits::const_pointer;
	using reference = typename TTraits::reference;
	using const_reference = typename TTraits::const_reference;
	using size_type = typename TTraits::size_type;
	using difference_type = typename TTraits::difference_type;

public:
	deque_reverse_iterator() noexcept
		: m_it{ nullptr }
	{}
	deque_reverse_iterator(iterator it) noexcept
		: m_it{ it }
	{}
	deque_reverse_iterator(deque_reverse_iterator&& other) noexcept = default;
	deque_reverse_iterator(deque_reverse_iterator const& other) noexcept = default;
	deque_reverse_iterator& operator=(deque_reverse_iterator&& other) noexcept = default;
	deque_reverse_iterator& operator=(deque_reverse_iterator const& other) noexcept = default;
	~deque_reverse_iterator() noexcept = default;
	deque_reverse_iterator& operator++() noexcept
	{
		++m_it;
		return *this;
	}
	deque_reverse_iterator operator++(int) const noexcept
	{
		return m_it++;
	}
	deque_reverse_iterator operator+(difference_type offset) const noexcept
	{
		return m_it + offset;
	}
	deque_reverse_iterator& operator+=(difference_type offset) const noexcept
	{
		m_it += offset;
		return *this;
	}
	deque_reverse_iterator& operator--() noexcept
	{
		--m_it;
		return *this;
	}
	deque_reverse_iterator operator--(int) const noexcept
	{
		return m_it--;
	}
	difference_type operator-(deque_reverse_iterator rhs) const noexcept
	{
		return m_it - rhs;
	}
	deque_reverse_iterator operator-(difference_type offset) const noexcept
	{
		return m_it - offset;
	}
	deque_reverse_iterator& operator-=(difference_type offset) const noexcept
	{
		m_it -= offset;
		return *this;
	}
	reference operator*() noexcept
	{
		return *(*m_it);
	}
	const_reference operator*() const noexcept
	{
		return *(*m_it);
	}
	pointer const operator->() noexcept
	{
		return *m_it;
	}
	const_pointer operator->() const noexcept
	{
		return *m_it;
	}
	bool operator==(deque_reverse_iterator const& other) const noexcept
	{
		return m_it == other.m_it;
	}
	bool operator!=(deque_reverse_iterator const& other) const noexcept
	{
		return m_it == other.m_it;
	}
private:
	template <typename U, typename W>
	friend class deque_reverse_const_iterator;
private:
	iterator m_it;
};
template <typename T, typename TTraits>
class deque_reverse_const_iterator
{
public:
	using iterator = vector_reverse_const_iterator<T, TTraits>;
	using value_type = typename TTraits::value_type;
	using pointer = typename TTraits::pointer;
	using const_pointer = typename TTraits::const_pointer;
	using reference = typename TTraits::reference;
	using const_reference = typename TTraits::const_reference;
	using size_type = typename TTraits::size_type;
	using difference_type = typename TTraits::difference_type;

	deque_reverse_const_iterator() noexcept
		: m_it{ nullptr }
	{}
	deque_reverse_const_iterator(iterator it) noexcept
		: m_it{ it }
	{}
	deque_reverse_const_iterator(deque_reverse_iterator<T, TTraits>&& other) noexcept
		: m_it{ other.m_it }
	{}
	deque_reverse_const_iterator(deque_reverse_iterator<T, TTraits> const&) noexcept
		: m_it{ other.m_it }
	{}
	deque_reverse_const_iterator(deque_reverse_const_iterator&& other) noexcept = default;
	deque_reverse_const_iterator(deque_reverse_const_iterator const& other) noexcept = default;
	deque_reverse_const_iterator& operator=(deque_reverse_const_iterator&& other) noexcept = default;
	deque_reverse_const_iterator& operator=(deque_reverse_const_iterator const& other) noexcept = default;
	~deque_reverse_const_iterator() noexcept = default;
	deque_reverse_const_iterator& operator++() noexcept
	{
		++m_it;
		return *this;
	}
	deque_reverse_const_iterator operator++(int) const noexcept
	{
		return m_it++;
	}
	deque_reverse_const_iterator operator+(difference_type offset) const noexcept
	{
		return m_it + offset;
	}
	deque_reverse_const_iterator& operator+=(difference_type offset) const noexcept
	{
		m_it += offset;
		return *this;
	}
	deque_reverse_const_iterator& operator--() noexcept
	{
		--m_it;
		return *this;
	}
	deque_reverse_const_iterator operator--(int) const noexcept
	{
		return m_it--;
	}
	difference_type operator-(deque_reverse_const_iterator rhs) const noexcept
	{
		return m_it - rhs;
	}
	deque_reverse_const_iterator operator-(difference_type offset) const noexcept
	{
		return m_it - offset;
	}
	deque_reverse_const_iterator& operator-=(difference_type offset) const noexcept
	{
		m_it -= offset;
		return *this;
	}
	const_reference operator*() const noexcept
	{
		return *(*m_it);
	}
	const_pointer operator->() const noexcept
	{
		return *m_it;
	}
	bool operator==(deque_reverse_const_iterator const& other) const noexcept
	{
		return m_it == other.m_it;
	}
	bool operator!=(deque_reverse_const_iterator const& other) const noexcept
	{
		return m_it == other.m_it;
	}
private:
	iterator m_it;
};
template <typename T, typename TTraits>
class deque_iterator
{
public:
	using iterator = typename vector_iterator<T, TTraits>;
	using value_type = typename TTraits::value_type;
	using pointer = typename TTraits::pointer;
	using const_pointer = typename TTraits::const_pointer;
	using reference = typename TTraits::reference;
	using const_reference = typename TTraits::const_reference;
	using size_type = typename TTraits::size_type;
	using difference_type = typename TTraits::difference_type;

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
	reference operator*() noexcept
	{
		return *(*m_it);
	}
	const_reference operator*() const noexcept
	{
		return *(*m_it);
	}
	pointer operator->() noexcept
	{
		return *m_it;
	}
	const_pointer operator->() const noexcept
	{
		return *m_it;
	}
	bool operator==(deque_iterator const& other) const noexcept
	{
		return m_it == other.m_it;
	}
	bool operator!=(deque_iterator const& other) const noexcept
	{
		return m_it != other.m_it;
	}
private:
	template <typename U, typename W>
	friend class deque_const_iterator;
private:
	iterator m_it;
};
template <typename T, typename TTraits>
class deque_const_iterator
{
public:
	using iterator = typename vector_const_iterator<T, TTraits>;
	using value_type = typename TTraits::value_type;
	using pointer = typename TTraits::pointer;
	using const_pointer = typename TTraits::const_pointer;
	using reference = typename TTraits::reference;
	using const_reference = typename TTraits::const_reference;
	using size_type = typename TTraits::size_type;
	using difference_type = typename TTraits::difference_type;

	deque_const_iterator() noexcept
		: m_it{ nullptr }
		, m_size{ 0 }
	{}
	deque_const_iterator(iterator it) noexcept
		: m_it{ it }
	{}
	deque_const_iterator(deque_iterator<T, TTraits> const& other) noexcept
		: m_it{ other.m_it }
	{
	}
	deque_const_iterator(deque_iterator<T, TTraits>&& other) noexcept
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
	const_reference operator*() const noexcept
	{
		return *(*m_it);
	}
	const_pointer operator->() const noexcept
	{
		return *m_it;
	}
	bool operator==(deque_const_iterator const& other) const noexcept
	{
		return m_it == other.m_it;
	}
	bool operator!=(deque_const_iterator const& other) const noexcept
	{
		return m_it == other.m_it;
	}
private:
	iterator m_it;
};
}
}

namespace std
{
template <typename T, typename TTraits>
struct iterator_traits<agl::impl::deque_iterator<T, TTraits>>
{
	using value_type = typename TTraits::value_type;
	using pointer = typename TTraits::pointer;
	using reference = typename TTraits::reference;
	using difference_type = typename TTraits::difference_type;
	using iterator_category = random_access_iterator_tag;

};

template <typename T, typename TTraits>
struct iterator_traits<agl::impl::deque_const_iterator<T, TTraits>>
{
	using value_type = typename TTraits::value_type;
	using pointer = typename TTraits::const_pointer;
	using reference = typename TTraits::const_reference;
	using difference_type = typename TTraits::difference_type;
	using iterator_category = random_access_iterator_tag;
};
}