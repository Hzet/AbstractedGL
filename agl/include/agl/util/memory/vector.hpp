#pragma once
#include "agl/util/memory/pool.hpp"
#include "agl/util/vector.hpp"

namespace agl
{
namespace mem
{
template <typename T>
using vector = agl::vector<T, pool::allocator<T>>;
}
}