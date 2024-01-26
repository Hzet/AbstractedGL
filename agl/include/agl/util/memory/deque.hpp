#pragma once
#include "agl/util/deque.hpp"
#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
template <typename T>
using deque = agl::deque<T, pool::allocator<T>>;
}
}