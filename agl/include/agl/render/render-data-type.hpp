#pragma once
#include <cstdint>

namespace agl
{
enum render_status
{
	INVALID,
	CHANGED,
	READY
};

enum render_data_type
{
	DOUBLE,
	DVEC2,
	DVEC3,
	DVEC4,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	UINT64,
	UVEC2,
	UVEC3,
	UVEC4,
};

std::uint64_t get_render_data_type_size(render_data_type data_type);
std::uint64_t get_render_data_type_count(render_data_type data_type);
}