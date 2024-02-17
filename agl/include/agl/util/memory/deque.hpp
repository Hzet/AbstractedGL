#pragma once
#include "agl/util/deque.hpp"
#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
/**
 * @brief 
 * Shares properties with 'deque', but is using 'pool::allocator' instead.
 * @tparam T 
 */
template <typename T>
using deque = agl::deque<T, pool::allocator<T>>;
}
}