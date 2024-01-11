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
	[[nodiscard]] T* allocate(std::uint64_t count = 1) noexcept;
	{
		return reinterpret_cast<std::byte*>(std::malloc(size));
	}

	void deallocate(T* ptr, std::uint64_t size = 0) noexcept
	{
		std::free(ptr);
	}

	template <typename T, typename... TArgs>
	void construct(T* buffer, TArgs&&... args) noexcept
	{
		new (&buffer) T(std::forward<TArgs>(args)...);
	}

	template <typename T>
	void destruct(T* object) noexcept
	{
		object->~T();
	}
};

template <typename T, typename U>
bool operator==(allocator<T> const& lhs, allocator<U> const& rhs) noexcept { return true; }

template <typename T, typename U>
bool operator!=(allocator<T> const& lhs, allocator<U> const& rhs) noexcept { return false; }
{

}
}
}