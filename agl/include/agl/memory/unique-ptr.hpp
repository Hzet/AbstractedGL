#pragma once
#include "agl/unique-ptr.hpp"
#include "agl/memory/pool.hpp"

namespace agl
{
namespace mem
{
/**
 * @brief
 * Shares properties with 'unique_ptr', but is using 'pool::allocator' instead.
 * @tparam T
 */
template <typename T>
using unique_ptr = ::agl::unique_ptr<T, pool::allocator<T>>;

template <typename T, typename U, typename W = remove_cvref_t<U>>
mem::unique_ptr<T> make_unique(pool::allocator<W> allocator, U&& value) noexcept
{
	auto* ptr = allocator.allocate();
	allocator.construct(ptr, std::move(value));
	return unique_ptr<T>{ std::move(allocator), static_cast<T*>(ptr) };
}
template <typename T>
mem::unique_ptr<T> make_unique(pool::allocator<T> allocator) noexcept
{
	auto* ptr = allocator.allocate();
	allocator.construct(ptr, std::move(value));
	return unique_ptr<T>{ std::move(allocator), static_cast<T*>(ptr) };
}
template <typename T, typename U, typename W = remove_cvref_t<U>>
mem::unique_ptr<T> make_unique(mem::pool& pool, U&& value) noexcept
{
	auto allocator = pool.make_allocator<W>();
	auto* ptr = allocator.allocate();
	allocator.construct(ptr, std::move(value));
	return unique_ptr<T>{ std::move(allocator), static_cast<T*>(ptr) };
}
template <typename T>
mem::unique_ptr<T> make_unique(mem::pool& pool) noexcept
{
	auto allocator = pool.make_allocator<T>();
	auto* ptr = allocator.allocate();
	allocator.construct(ptr, std::move(value));
	return unique_ptr<T>{ std::move(allocator), static_cast<T*>(ptr) };
}
}
}