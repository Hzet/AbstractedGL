#pragma once
#include <unordered_map>
#include <vector>

#include "agl/util/memory/pool.hpp"

namespace agl
{
namespace mem
{
template <typename TKey, typename T>
using unordered_map = std::unordered_map<TKey, T, std::hash<TKey>, std::equal_to<TKey>, pool::allocator<std::pair<TKey const, T>>>;

template <typename T>
using vector = std::vector<T, pool::allocator<T>>;
}
}