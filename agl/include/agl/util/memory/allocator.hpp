#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>

namespace agl
{
namespace mem
{
class allocator
{
public:
	std::byte* allocate(std::uint64_t size) const noexcept
	{
		return reinterpret_cast<std::byte*>(std::malloc(size));
	}

	void deallocate(std::byte* ptr, std::uint64_t size=0) const noexcept
	{
		std::free(ptr);
	}

	template <typename T, typename... TArgs>
	void construct(std::byte* buffer, TArgs&&... args) const noexcept
	{
		new (&buffer) T(std::forward<TArgs>(args)...);
	}

	template <typename T>
	std::byte* destruct(T* object) noexcept
	{
		object->~T();
		return reinterpret_cast<std::byte*>(object);
	}

	template <typename T>
	bool align(std::byte* buffer, std::uint64_t count) noexcept
	{
		std::uint64_t space = 0;
		std::align(alignof(T), count * sizeof(T), buffer, space);
	}
};
}
}