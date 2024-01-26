#pragma once
#include "agl/util/unique-ptr.hpp"
#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
template <typename T>
using unique_ptr = ::agl::unique_ptr<T, pool::allocator<T>>;

template <typename T, typename U>
unique_ptr<T> make_unique(pool::allocator<T> const& allocator, U&& value) noexcept
{
	auto* ptr = allocator.allocate();
	allocator.construct(ptr, std::move(value));
	return unique_ptr<T>{ allocator, ptr };
}
}
}