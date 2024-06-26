#pragma once
#include "agl/memory/allocator.hpp"

namespace agl
{
/**
 * @brief 
 * Shares properties with 'std::unique_ptr'.
 * @tparam T 
 * @tparam TAlloc 
 */
template <typename T, typename TAlloc = mem::allocator<T>>
class unique_ptr final
{
public:
	using allocator_type = typename TAlloc::template rebind<T>;
	using value_type = typename TAlloc::value_type;
	using pointer = typename TAlloc::pointer;
	using const_pointer = typename TAlloc::const_pointer;
	using reference = typename TAlloc::reference;
	using const_reference = typename TAlloc::const_reference;
	using size_type = typename TAlloc::size_type;

	unique_ptr(allocator_type allocator = {}, pointer ptr = nullptr)
		: m_allocator{ std::move(allocator) }
		, m_data{ ptr }
	{
	}

	unique_ptr(unique_ptr&& other)
		: m_allocator{ std::move(other.m_allocator) }
		, m_data{ other.m_data }
	{
		other.m_data = nullptr;
	}

	template <typename U>//, typename TEnable = std::enable_if_t<std::is_same_v<T, U>>>
	unique_ptr(unique_ptr&& other)
		: m_allocator{ std::move(other.m_allocator) }
		, m_data{ other.m_data }
	{
		other.m_data = nullptr;
	}

	unique_ptr& operator=(unique_ptr&& other)
	{
		if (this == &other)
			return *this;

		m_allocator = std::move(other.m_allocator);
		m_data = other.m_data;
		other.m_data = nullptr;

		return *this;
	}

	template <typename U>//, typename TEnable = std::enable_if_t<std::is_same_v<T, U>>>
	unique_ptr& operator=(unique_ptr<U>&& other)
	{
		if (this == &other)
			return *this;

		m_allocator = std::move(other.m_allocator);
		m_data = other.m_data;
		other.m_data = nullptr;

		return *this;
	}

	unique_ptr& operator=(std::nullptr_t)
	{
		reset();
		return *this;
	}

	unique_ptr(unique_ptr const&) = delete;
	unique_ptr& operator=(unique_ptr const&) = delete;
	~unique_ptr()
	{
		release();
	}

	void release()
	{
		if (m_data == nullptr)
			return;

		m_allocator.destruct(m_data);
		m_allocator.deallocate(m_data);
	}

	template <typename T, typename... TArgs>
	void reset(T arg, TArgs&&... args)
	{
		if (m_data != nullptr)
			m_allocator.destruct(m_data);

		m_data = m_allocator.construct(std::forward<T>(arg), std::forward<TArgs>(args...));
	}

	void reset(std::nullptr_t = nullptr)
	{
		if (m_data != nullptr)
			m_allocator.destruct(m_data);
		m_data = nullptr;
	}

	void swap(unique_ptr& other)
	{
		std::swap(m_allocator, other.m_allocator);
		std::swap(m_data, other.m_data);
	}

	pointer get()
	{
		return m_data;
	}

	const_pointer get() const
	{
		return m_data;
	}

	operator bool() const
	{
		return m_data != nullptr;
	}

	reference operator*()
	{
		AGL_ASSERT(m_data != nullptr, "invalid pointer");

		return *m_data;
	}

	const_reference operator*() const
	{
		AGL_ASSERT(m_data != nullptr, "invalid pointer");

		return *m_data;
	}

	pointer operator->()
	{
		AGL_ASSERT(m_data != nullptr, "invalid pointer");
		
		return m_data;
	}

	const_pointer operator->() const
	{
		AGL_ASSERT(m_data != nullptr, "invalid pointer");
		
		return m_data;
	}

	bool operator==(std::nullptr_t) const
	{
		return m_data == nullptr;
	}

	bool operator!=(std::nullptr_t) const
	{
		return m_data != nullptr;
	}

private:
	allocator_type m_allocator;
	pointer m_data;
};

template <typename T>
unique_ptr<T> make_unique()
{
	static_assert(!std::is_reference_v<T>, "invalid type");
	
	using type = std::remove_const_t<T>;
	auto alloc = unique_ptr<type>::allocator_type{};
	auto* ptr = alloc.allocate();
	alloc.construct(ptr);
	return unique_ptr<T>{ std::move(alloc), static_cast<T*>(ptr) };
}

template <typename T, typename U>
unique_ptr<T> make_unique(U&& value)
{
	using type = std::remove_cv_t<std::remove_reference_t<U>>;
	auto allocator = mem::allocator<type>{};
	auto* ptr = allocator.allocate();
	allocator.construct(ptr, std::move(value));
	return unique_ptr<T>{ std::move(allocator), static_cast<T*>(ptr) };
}
}