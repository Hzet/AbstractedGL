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

	template <typename... TArgs>
	static unique_ptr allocate(TArgs&&... args)
	{
		return allocate_a(allocator_type{}, std::forward<TArgs>(args)...);
	}

	template <typename... TArgs>
	static unique_ptr allocate_a(allocator_type allocator, TArgs&&... args)
	{
		auto* ptr = allocator.allocate();
		allocator.construct(ptr, std::forward<TArgs>(args)...);
		return unique_ptr{ std::move(allocator), ptr };
	}

	template <typename U, typename... TArgs>
	static unique_ptr polymorphic(TArgs&&... args)
	{
		return polymorphic_a<U>(allocator_type{}, std::forward<TArgs>(args)...);
	}

	template <typename U, typename... TArgs>
	static unique_ptr polymorphic_a(typename allocator_type::template rebind<U> allocator, TArgs&&... args)
	{
		auto* ptr = allocator.allocate();
		allocator.construct(ptr, std::forward<TArgs>(args)...);
		return unique_ptr{ std::move(allocator), ptr };
	}

	unique_ptr(allocator_type allocator = {}, pointer ptr = nullptr)
		: m_allocator{ std::move(allocator) }
		, m_data{ ptr }
	{
	}

	unique_ptr(unique_ptr&& other) noexcept
		: m_allocator{ std::move(other.m_allocator) }
		, m_data{ other.m_data }
	{
		other.m_data = nullptr;
	}

	template <typename U>//, typename TEnable = std::enable_if_t<std::is_same_v<T, U>>>
	unique_ptr(unique_ptr&& other) noexcept
		: m_allocator{ std::move(other.m_allocator) }
		, m_data{ other.m_data }
	{
		other.m_data = nullptr;
	}

	unique_ptr& operator=(unique_ptr&& other) noexcept
	{
		if (this == &other)
			return *this;

		release();
		m_allocator = std::move(other.m_allocator);
		m_data = other.m_data;
		other.m_data = nullptr;

		return *this;
	}

	template <typename U>//, typename TEnable = std::enable_if_t<std::is_same_v<T, U>>>
	unique_ptr& operator=(unique_ptr<U>&& other) noexcept
	{
		if (this == &other)
			return *this;

		release();
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
		m_data = nullptr;
	}

	template <typename U>
	void reset(U* ptr)
	{
		if (m_data != nullptr)
			release();

		m_data = ptr;
	}

	void reset(std::nullptr_t = nullptr)
	{
		release();
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

	bool operator==(pointer rhs) const
	{
		return m_data == rhs;
	}

	bool operator!=(std::nullptr_t) const
	{
		return m_data != nullptr;
	}

private:
	allocator_type m_allocator;
	pointer m_data;
};
}