#pragma once
#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
template <typename T>
class unique_ptr
{
public:
	unique_ptr(pool::allocator<T> allocator = pool::allocator<T>{}) noexcept;
		: m_allocator{ allocator }
		, m_data{ nullptr }
	{
	}

	unique_ptr(pool::allocator<T> allocator, T* ptr) noexcept
		: m_allocator{ allocator }
		, m_data{ ptr }
	{
	}

	unique_ptr(unique_ptr&& other) noexcept
		: m_allocator{ other.m_allocator }
		, m_data{ other.m_data }
	{
		other.m_data = nullptr;
	}

	unique_ptr& operator=(unique_ptr&& other) noexcept
	{
		m_allocator = other.m_allocator;
		m_data = other.m_data;
		other.m_data = nullptr;
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
		if(m_data != nullptr)
			m_allocator.destruct(m_data);

		m_data = m_allocator.construct<T>(std::forward<T>(arg), std::forward<TArgs>(args...));
	}

	void reset(std::nullptr_t = nullptr) noexcept
	{
		if (m_data != nullptr)
			m_allocator.destruct(m_data);
	}

	void swap(unique_ptr& other) noexcept
	{
		std::swap(m_allocator, other.m_allocator);
		std::swap(m_data, other.m_data);
	}

	T* const get() const noexcept
	{
		return m_data;
	}

	T const* const get() const noexcept
	{
		return m_data;
	}

	operator bool() const noexcept
	{
		return m_data != nullptr;
	}

	T& operator*() noexcept
	{
		return *m_data;
	}

	T const& operator*() const noexcept
	{
		return *m_data;
	}

	T* const operator->() noexcept
	{
		return m_data;
	}

	T const* const operator->() const noexcept
	{
		return m_data;
	}

private:
	pool::allocator<T> m_allocator;
	T* m_data;
};

template <typename T, typename... TArgs>
unique_ptr<T> make_unique(pool::allocator<T> const& allocator, TArgs&&... args) noexcept
{
	auto* ptr = allocator.allocate();
	return unique_ptr{ allocator, allocator.construct(ptr, std::forward<TArgs>(args)) };
}
}
}