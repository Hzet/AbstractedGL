template <typename T>
pool::allocator<T>::allocator(pool* ptr) noexcept
	: m_block{ ptr }
{
}

template <typename T>
template <typename U>
constexpr pool::allocator<T>::allocator(allocator<U> const& other)
	: m_block{ other.m_block }
{}

template <typename T>
T* pool::allocator<T>::allocate(std::uint64_t count) noexcept
{
	return reinterpret_cast<T*>(m_block->allocate(count * sizeof(T)));
}

template <typename T>
void pool::allocator<T>::deallocate(T* ptr, std::uint64_t count) noexcept
{
	m_block->deallocate(reinterpret_cast<std::byte*>(ptr), count * sizeof(T));
}

template <typename T>
pool::allocator<T> pool::make_allocator() noexcept
{
	return allocator<T>{ this };
}

template <typename U, typename W>
bool operator==(pool::allocator<U> const& lhs, pool::allocator<W> const& rhs) noexcept
{
	return lhs.m_block == rhs.m_block;
}

template <typename U, typename W>
bool operator!=(pool::allocator<U> const& lhs, pool::allocator<W> const& rhs) noexcept
{
	return lhs.m_block != rhs.m_block;
}