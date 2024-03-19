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

template <typename T, typename U, typename W>
mem::unique_ptr<T> make_unique(pool::allocator<W> allocator, U&& value)
{
	using type = remove_cvref_t<U>;
	auto true_allocator = allocator.rebind_copy<type>();
	auto* ptr = true_allocator.allocate();
	true_allocator.construct(ptr, std::forward<type>(value));
	return unique_ptr<T>{ std::move(true_allocator), static_cast<T*>(ptr) };
}
template <typename T, typename U>
mem::unique_ptr<T> make_unique(pool::allocator<U> allocator)
{
	using type = remove_cvref_t<T>;
	auto true_allocator = allocator.rebind_copy<type>();
	auto* ptr = true_allocator.allocate();
	true_allocator.construct(ptr);
	return unique_ptr<T>{ std::move(true_allocator), static_cast<T*>(ptr) };
}
template <typename T, typename U>
mem::unique_ptr<T> make_unique(mem::pool& pool, U&& value)
{
	using type = remove_cvref_t<U>;
	auto allocator = pool.make_allocator<type>();
	auto* ptr = allocator.allocate();
	allocator.construct(ptr, std::forward<type>(value));
	return unique_ptr<T>{ std::move(allocator), static_cast<T*>(ptr) };
}
template <typename T>
mem::unique_ptr<T> make_unique(mem::pool& pool)
{
	auto allocator = pool.make_allocator<T>();
	auto* ptr = allocator.allocate();
	allocator.construct(ptr);
	return unique_ptr<T>{ std::move(allocator), static_cast<T*>(ptr) };
}
}
}