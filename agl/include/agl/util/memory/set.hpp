#pragma once
#include "agl/util/set.hpp"
#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
template <typename T, typename TComp = std::less<T>>
using set = ::agl::set<T, TComp, pool::allocator<T>>;
}
}