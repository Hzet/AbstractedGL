template <typename T>
pool::allocator<T>::allocator(pool* ptr) noexcept
	: m_pool{ ptr }
{
}

template <typename T>
template <typename U>
pool::allocator<T>::allocator(allocator<U> const& other) noexcept
	: m_pool{ other.m_pool }
{
}

template <typename T>
template <typename U>
pool::allocator<T>& pool::allocator<T>::operator=(allocator<U> const& other) noexcept
{
	AGL_ASSERT(m_pool == nullptr || *this == other, "allocators are not equal");

	m_pool = other.m_pool;
}

template <typename T>
T* pool::allocator<T>::allocate(std::uint64_t count) noexcept
{
	AGL_ASSERT( m_pool != nullptr, "pool handle is nullptr");

	return reinterpret_cast<T*>(m_pool->allocate(count * sizeof(T)));
}

template <typename T>
template <typename... TArgs>
T* pool::allocator<T>::construct(T* buffer, TArgs&&... args) noexcept
{
	AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");
	AGL_ASSERT(buffer != nullptr, "buffer handle is nullptr");

	return new (buffer) T(std::forward<TArgs>(args)...);
}

template <typename T>
void pool::allocator<T>::deallocate(T* ptr, std::uint64_t count) noexcept
{
	AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");

	m_pool->deallocate(reinterpret_cast<std::byte*>(ptr), count * sizeof(T));
}

template <typename T>
void pool::allocator<T>::destruct(T* ptr) noexcept
{
	AGL_ASSERT(m_pool != nullptr, "pool handle is nullptr");
	AGL_ASSERT(ptr != nullptr, "ptr handle is nullptr");
	
	ptr->~T();
}

template <typename T>
pool::allocator<T> pool::make_allocator() noexcept
{
	return allocator<T>{ this };
}

template <typename U, typename W>
bool operator==(pool::allocator<U> const& lhs, pool::allocator<W> const& rhs) noexcept
{
	return lhs.m_pool == nullptr || lhs.m_pool == rhs.m_pool;
}

template <typename U, typename W>
bool operator!=(pool::allocator<U> const& lhs, pool::allocator<W> const& rhs) noexcept
{
	return lhs.m_pool != nullptr && lhs.m_pool != rhs.m_pool;
}