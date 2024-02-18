#pragma once
#include "agl/set.hpp"
#include "agl/memory/pool.hpp"

namespace agl
{
namespace mem
{
/**
 * @brief
 * Shares properties with 'set', but is using 'pool::allocator' instead.
 * @tparam T
 */
template <typename T, typename TComp = std::less<T>>
using set = ::agl::set<T, TComp, pool::allocator<T>>;
}
}