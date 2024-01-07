template <typename T>
pool::allocator<T>::allocator(pool* ptr) noexcept
	: m_pool{ ptr }
{
}

template <typename T>
pool::allocator<T>::allocator(allocator&& other)
	: m_pool{ other.m_pool }
{
	AGL_ASSERT(*this == other, "allocators are not equal");
}

template <typename T>
pool::allocator<T>& pool::allocator<T>::operator=(allocator&& other)
{
	AGL_ASSERT(*this == other, "allocators are not equal");

	m_pool = other.m_pool;
}

template <typename T>
pool::allocator<T>::allocator(allocator const& other)
	: m_pool{ other.m_pool }
{
	AGL_ASSERT(*this == other, "allocators are not equal");
}

template <typename T>
pool::allocator<T>& pool::allocator<T>::operator=(allocator const& other)
{
	AGL_ASSERT(*this == other, "allocators are not equal");

	m_pool = other.m_pool;
}

template <typename T>
T* pool::allocator<T>::allocate(std::uint64_t count) noexcept
{
	return reinterpret_cast<T*>(m_pool->allocate(count * sizeof(T)));
}

template <typename T>
template <typename... TArgs>
T* pool::allocator<T>::construct(T* buffer, TArgs&&... args) const noexcept
{
	return new (&buffer) T(std::forward<TArgs>(args...));
}

template <typename T>
void pool::allocator<T>::deallocate(T* ptr, std::uint64_t count) noexcept
{
	m_pool->deallocate(reinterpret_cast<std::byte*>(ptr), count * sizeof(T));
}

template <typename T>
void pool::allocator<T>::destruct(T* ptr) const noexcept
{
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
	return lhs.m_pool == rhs.m_pool;
}

template <typename U, typename W>
bool operator!=(pool::allocator<U> const& lhs, pool::allocator<W> const& rhs) noexcept
{
	return lhs.m_pool != rhs.m_pool;
}