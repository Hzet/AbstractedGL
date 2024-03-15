#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include "agl/core/debug.hpp"

namespace agl
{
template <typename T>
class type_traits
{
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = T const*;
	using reference = T&;
	using const_reference = T const&;
	using size_type = std::uint64_t;
	using difference_type = std::ptrdiff_t;
};
namespace mem
{
/**
 * @brief 
 * Stateless allocator. Allocates the memory calling 'std::malloc', deallocates with 'std::free'.
 * Allocated buffers are aligned according to the alignment of 'T', but no objects are constructed nor deleted upon calling 'construct' or 'destruct' respectively.
 * @tparam T 
 */
template <typename T>
class allocator
{
public:
	static_assert(!std::is_const_v<T>, "allocator<const T> is ill-formed");
	static_assert(!std::is_function_v<T>, "[allocator.requirements]");
	static_assert(!std::is_reference_v<T>, "[allocator.requirements]");

public:
	using value_type = typename type_traits<T>::value_type;
	using pointer = typename type_traits<T>::pointer;
	using const_pointer = typename type_traits<T>::const_pointer;
	using reference = typename type_traits<T>::reference;
	using const_reference = typename type_traits<T>::const_reference;
	using size_type = typename type_traits<T>::size_type;
	using difference_type = typename type_traits<T>::difference_type;
	template <typename U>
	using rebind = allocator<U>;

public:
	allocator() 
	{
		m_alloc_count = 0;
	}
	allocator(allocator const&)
	{
		m_alloc_count = 0;
	}
	template <typename U>
	allocator(allocator<U> const&)
	{
		m_alloc_count = 0;
	}
	allocator(allocator&& other)
	{
		m_alloc_count = other.m_alloc_count;
		other.m_alloc_count = 0;
	}
	template <typename U>
	allocator(allocator<U>&& other)
	{
		m_alloc_count = other.m_alloc_count;
		other.m_alloc_count = 0;
	}
	template <typename U>
	allocator& operator=(allocator<U>&& other)
	{
		if (this == &other)
			return *this;

		m_alloc_count = other.m_alloc_count;
		other.m_alloc_count = 0;
		return *this;
	}
	template <typename U>
	allocator& operator=(allocator<U> const& other) 
	{
		if (this == &other)
			return *this;

		m_alloc_count = 0;
		return *this;
	}
	~allocator() 
	{
		AGL_ASSERT(m_alloc_count == 0, "some objects were not destroyed");
	}

	[[nodiscard]] pointer allocate(size_type count = 1, std::uint64_t alignment = alignof(value_type))
	{
		auto const size = count * sizeof(value_type);
		auto* ptr = std::malloc(size);
		auto i = size;
		std::align(alignment, size, ptr, i);
		m_alloc_count += count;
		return reinterpret_cast<pointer>(ptr);
	}
	void deallocate(pointer ptr, size_type count = 1)
	{
		AGL_ASSERT(m_alloc_count > 0, "invalid deallocation call");

		m_alloc_count -= count;
		std::free(ptr);
	}
	template <typename... TArgs>
	void construct(pointer buffer, TArgs&&... args)
	{
		AGL_ASSERT(m_alloc_count > 0, "invalid construction call");

		new (buffer) value_type(std::forward<TArgs>(args)...);
	}
	void destruct(pointer ptr)
	{
		AGL_ASSERT(m_alloc_count > 0, "invalid destruction call");

		ptr->~T();
	}

private:
	template <typename U>
	friend class allocator;

private:
	std::uint64_t m_alloc_count;
};

template <typename T, typename U>
bool operator==(allocator<T> const& lhs, allocator<U> const& rhs) { return true; }

template <typename T, typename U>
bool operator!=(allocator<T> const& lhs, allocator<U> const& rhs) { return false; }
}
}