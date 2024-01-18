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
	allocator() noexcept {}
	template <typename U>
	allocator(allocator<U> const&) noexcept {}
	template <typename U>
	allocator& operator=(allocator<U> const&) noexcept {}
	~allocator() noexcept {}

	[[nodiscard]] pointer allocate(size_type count = 1) noexcept
	{
		return reinterpret_cast<pointer>(std::malloc(count * sizeof(value_type)));
	}
	void deallocate(pointer ptr, size_type size = 0) noexcept
	{
		std::free(ptr);
	}
	template <typename... TArgs>
	void construct(pointer buffer, TArgs&&... args) noexcept
	{
		new (buffer) value_type(std::forward<TArgs>(args)...);
	}
	void destruct(pointer ptr) noexcept
	{
		ptr->~T();
	}
};

template <typename T, typename U>
bool operator==(allocator<T> const& lhs, allocator<U> const& rhs) noexcept { return true; }

template <typename T, typename U>
bool operator!=(allocator<T> const& lhs, allocator<U> const& rhs) noexcept { return false; }
}
}