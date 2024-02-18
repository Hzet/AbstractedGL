#pragma once
#include "agl/memory/pool.hpp"
#include "agl/vector.hpp"

namespace agl
{
namespace mem
{
/**
 * @brief
 * Shares properties with 'vector', but is using 'pool::allocator' instead.
 * @tparam T
 */
template <typename T>
using vector = agl::vector<T, pool::allocator<T>>;
}
}