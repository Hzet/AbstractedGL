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
	allocator() noexcept 
		: m_object_count{ 0 }
	{
	}
	template <typename U>
	allocator(allocator<U> const&) noexcept
		: m_object_count{ 0 }
	{
	}
	template <typename U>
	allocator(allocator<U>&& other) noexcept
		: m_object_count{ other.m_object_count }
	{
		other.m_object_count = 0;
	}
	template <typename U>
	allocator& operator=(allocator<U>&& other) noexcept
	{
		if (this == &other)
			return *this;

		m_object_count = other.m_object_count;
		other.m_object_count = 0;
		return *this;
	}
	template <typename U>
	allocator& operator=(allocator<U> const& other) noexcept 
	{
		if (this == &other)
			return *this;

		m_object_count = 0;
		return *this;
	}
	~allocator() noexcept 
	{
		AGL_ASSERT(m_object_count == 0, "some objects were not destroyed");
	}

	[[nodiscard]] pointer allocate(size_type count = 1, std::uint64_t alignment = alignof(value_type)) noexcept
	{
		auto const size = count * sizeof(value_type);
		auto* ptr = std::malloc(size);
		auto i = size;
		std::align(alignment, size, ptr, i);
		return reinterpret_cast<pointer>(ptr);
	}
	void deallocate(pointer ptr, size_type size = 0) noexcept
	{
		AGL_ASSERT(m_object_count == 0, "some objects were not destroyed");

		std::free(ptr);
	}
	template <typename... TArgs>
	void construct(pointer buffer, TArgs&&... args) noexcept
	{
		new (buffer) value_type(std::forward<TArgs>(args)...);
	}
	void destruct(pointer ptr) noexcept
	{
		AGL_ASSERT(m_object_count > 0, "invalid destruction call");

		ptr->~T();
	}

private:
	template <typename U>
	friend class allocator;

private:
	std::uint64_t m_object_count;
};

template <typename T, typename U>
bool operator==(allocator<T> const& lhs, allocator<U> const& rhs) noexcept { return true; }

template <typename T, typename U>
bool operator!=(allocator<T> const& lhs, allocator<U> const& rhs) noexcept { return false; }
}
}