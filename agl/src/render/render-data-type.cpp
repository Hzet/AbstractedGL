#include "agl/render/render-data-type.hpp"
#include "agl/render/opengl/call.hpp"
#include <glm/glm.hpp>

namespace agl
{
std::uint64_t get_render_data_type_size(render_data_type data_type)
{
	switch (data_type)
	{
	case DOUBLE: return sizeof(double);
	case DVEC2: return sizeof(glm::dvec2);
	case DVEC3: return sizeof(glm::dvec3);
	case DVEC4: return sizeof(glm::dvec4);
	case FLOAT: return sizeof(float);
	case VEC2: return sizeof(glm::vec2);
	case VEC3: return sizeof(glm::vec3);
	case VEC4: return sizeof(glm::vec4);
	case UINT64: return sizeof(std::uint64_t);
	case UVEC2: return sizeof(glm::uvec2);
	case UVEC3: return sizeof(glm::uvec2);
	case UVEC4: return sizeof(glm::uvec2);
	default: return 0;
	}
}
std::uint64_t get_render_data_type_count(render_data_type data_type)
{
	switch (data_type)
	{
	case DOUBLE: return 1;
	case DVEC2: return 2;
	case DVEC3: return 3;
	case DVEC4: return 4; 
	case FLOAT: return 1;
	case VEC2: return 2;
	case VEC3: return 3;
	case VEC4: return 4; 
	case UINT64: return 1;
	case UVEC2: return 2;
	case UVEC3: return 3;
	case UVEC4: return 4;
	default: return 0;
	}
}
}