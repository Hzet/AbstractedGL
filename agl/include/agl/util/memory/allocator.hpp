#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include "agl/core/debug.hpp"

namespace agl
{
namespace mem
{
template <typename T>
class allocator
{
public:
	AGL_STATIC_ASSERT(!std::is_const_v<T>, "allocator<const T> is ill-formed");
	AGL_STATIC_ASSERT(!std::is_function_v<T>, "[allocator.requirements]");
	AGL_STATIC_ASSERT(!std::is_reference_v<T>, "[allocator.requirements]");

public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = T const*;
	using reference = T&;
	using const_reference = T const&;
	using size_type = std::uint64_t;
	using difference_type = std::ptrdiff_t;

public:
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