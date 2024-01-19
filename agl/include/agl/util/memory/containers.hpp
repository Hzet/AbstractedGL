#pragma once
#include "agl/util/dictionary.hpp"
#include "agl/util/deque.hpp"
#include "agl/util/memory/pool.hpp"
#include "agl/util/vector.hpp"

namespace agl
{
namespace mem
{
template <typename TKey, typename TValue, typename TComp = std::less<TKey>>
using dictionary = agl::dictionary<TKey, TValue, TComp, pool::allocator<std::pair<TKey, TValue>>>;

template <typename T>
using deque = agl::deque<T, pool::allocator<T>>;

template <typename T>
using vector = agl::vector<T, pool::allocator<T>>;
}
}