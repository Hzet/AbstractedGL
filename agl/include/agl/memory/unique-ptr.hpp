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
}
}