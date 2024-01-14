#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>

namespace agl
{
namespace mem
{
template <typename T>
class allocator
{
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