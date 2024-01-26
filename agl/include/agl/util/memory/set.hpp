#pragma once
#include "agl/util/set.hpp"
#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
template <typename T, typename TComp>
using set = ::agl::set<T, TComp, pool::allocator<T>>;
}
}