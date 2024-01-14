#pragma once
#include "agl/util/dictionary.hpp"
#include "agl/util/memory/pool.hpp"
#include "agl/util/vector.hpp"

namespace agl
{
namespace mem
{
template <typename TKey, typename TValue>
using dictionary = agl::dictionary<TKey, TValue, pool::allocator<pair<TKey, TValue>>>;

template <typename T>
using vector = agl::vector<T, pool::allocator<T>>;
}
}