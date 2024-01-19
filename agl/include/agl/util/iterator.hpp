#pragma once
#include <type_traits>

namespace agl
{
namespace impl
{
template <typename... >
using void_t = void;

template <typename T, typename = void>
struct is_iterator 
    : std::false_type
{
    constexpr static bool value = false;
};

template <typename T>
struct is_iterator<T,
    void_t<decltype(++std::declval<T&>()),
        decltype(*std::declval<T&>()),
        std::enable_if_t<std::is_copy_assignable_v<T>>,
        std::enable_if_t<std::is_copy_constructible_v<T>>,
        std::enable_if_t<std::is_destructible_v<T>>>>
    : std::true_type
{
    using type = T;
    constexpr static bool value = true;
};

template <typename T>
using is_iterator_t = typename is_iterator<T>::type;

template <typename T>
constexpr auto is_iterator_v = typename is_iterator<T>::value;
}
}