#pragma once
#include "agl/util/memory/allocator.hpp"
#include "agl/util/memory/pool.hpp"

namespace agl
{
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

	unique_ptr(allocator_type allocator = {}, pointer ptr = nullptr) noexcept
		: m_allocator{ allocator }
		, m_data{ ptr }
	{
	}

	template <typename U, typename TEnable = std::enable_if_t<std::is_same_v<T, U>>>
	unique_ptr(unique_ptr<U>&& other) noexcept
		: m_allocator{ other.m_allocator }
		, m_data{ other.m_data }
	{
		other.m_data = nullptr;
	}

	template <typename U, typename TEnable = std::enable_if_t<std::is_same_v<T, U>>>
	unique_ptr& operator=(unique_ptr<U>&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_allocator = other.m_allocator;
		m_data = other.m_data;
		other.m_data = nullptr;

		return *this;
	}

	unique_ptr& operator=(std::nullptr_t) noexcept
	{
		reset();
		return *this;
	}

	unique_ptr(unique_ptr const&) noexcept = delete;
	unique_ptr& operator=(unique_ptr const&) noexcept = delete;
	~unique_ptr() noexcept
	{
		release();
	}

	void release() noexcept
	{
		if (m_data == nullptr)
			return;

		m_allocator.destruct(m_data);
		m_allocator.deallocate(m_data);
	}

	template <typename T, typename... TArgs>
	void reset(T arg, TArgs&&... args) noexcept
	{
		if (m_data != nullptr)
			m_allocator.destruct(m_data);

		m_data = m_allocator.construct(std::forward<T>(arg), std::forward<TArgs>(args...));
	}

	void reset(std::nullptr_t = nullptr) noexcept
	{
		if (m_data != nullptr)
			m_allocator.destruct(m_data);
		m_data = nullptr;
	}

	void swap(unique_ptr& other) noexcept
	{
		std::swap(m_allocator, other.m_allocator);
		std::swap(m_data, other.m_data);
	}

	pointer get() noexcept
	{
		return m_data;
	}

	const_pointer get() const noexcept
	{
		return m_data;
	}

	operator bool() const noexcept
	{
		return m_data != nullptr;
	}

	reference operator*() noexcept
	{
		return *m_data;
	}

	const_reference operator*() const noexcept
	{
		return *m_data;
	}

	pointer operator->() noexcept
	{
		return m_data;
	}

	const_pointer operator->() const noexcept
	{
		return m_data;
	}

	bool operator==(std::nullptr_t) const noexcept
	{
		return m_data == nullptr;
	}

private:
	allocator_type m_allocator;
	pointer m_data;
};

template <typename T, typename U>
unique_ptr<T> make_unique(U&& value) noexcept
{
	auto alloc = unique_ptr<U>::allocator_type{};
	auto* ptr = alloc.allocate();
	alloc.construct(ptr, std::move(value));
	return unique_ptr<T>{ alloc, ptr };
}
}