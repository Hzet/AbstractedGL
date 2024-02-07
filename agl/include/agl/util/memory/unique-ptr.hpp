#pragma once
#include "agl/util/unique-ptr.hpp"
#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
template <typename T>
using unique_ptr = ::agl::unique_ptr<T, pool::allocator<T>>;

template <typename T, typename U, typename W = std::remove_cv_t<std::remove_reference_t<U>>>
mem::unique_ptr<T> make_unique(pool::allocator<W> allocator, U&& value) noexcept
{
	auto* ptr = allocator.allocate();
	allocator.construct(ptr, std::move(value));
	return unique_ptr<T>{ std::move(allocator), static_cast<W*>(ptr) };
}
}
}