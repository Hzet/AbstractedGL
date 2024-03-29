#pragma once
#include "agl/dictionary.hpp"
#include "agl/memory/pool.hpp"

namespace agl
{
namespace mem
{
/**
 * @brief
 * Shares properties with 'dictionary', but is using 'pool::allocator' instead.
 * @tparam T
 */
template <typename TKey, typename TValue, typename TComp = std::less<TKey>>
using dictionary = agl::dictionary<TKey, TValue, TComp, pool::allocator<std::pair<TKey, TValue>>>;
}
}