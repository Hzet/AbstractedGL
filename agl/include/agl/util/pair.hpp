#pragma once

namespace agl
{
// TODO: learn what is compressed_pair and possibly use it here
template <typename T, typename U>
struct pair
{
	T first;
	U second;
};
}