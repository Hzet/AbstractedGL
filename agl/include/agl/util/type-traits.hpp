#pragma once
#include <type_traits>

namespace agl
{
template <typename T>
struct remove_cvref
{
	template <typename T>
	using type = std::remove_cv_t<std::remove_reference_t<T>>;
};
template <typename T>
using remove_cvref_t = remove_cvref<T>::type;
}