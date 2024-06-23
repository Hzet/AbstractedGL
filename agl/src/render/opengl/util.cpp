#include "agl/render/opengl/call.hpp"
#include "agl/render/opengl/util.hpp"
#include "agl/render/shader.hpp"

namespace agl
{
namespace opengl
{
std::uint32_t gl_render_data_type(render_data_type data_type)
{
	switch (data_type)
	{
	case DOUBLE: return GL_DOUBLE;
	case DVEC2:  return GL_DOUBLE;
	case DVEC3:  return GL_DOUBLE;
	case DVEC4:  return GL_DOUBLE;
	case FLOAT:  return GL_FLOAT;
	case VEC2:   return GL_FLOAT;
	case VEC3:   return GL_FLOAT;
	case VEC4:   return GL_FLOAT;
	case UINT64: return GL_UNSIGNED_BYTE;
	case UVEC2:  return GL_UNSIGNED_BYTE;
	case UVEC3:  return GL_UNSIGNED_BYTE;
	case UVEC4:  return GL_UNSIGNED_BYTE;
	default:     return GL_INVALID_ENUM;
	}
}

std::uint32_t gl_shader_data_type(shader_type type)
{
	switch (type)
	{
	case SHADER_COMPUTE:         return GL_COMPUTE_SHADER;
	case SHADER_FRAGMENT:        return	GL_FRAGMENT_SHADER;
	case SHADER_GEOMETRY:        return GL_GEOMETRY_SHADER;
	case SHADER_TESS_CONTROL:    return GL_TESS_CONTROL_SHADER;
	case SHADER_TESS_EVALUATION: return GL_TESS_EVALUATION_SHADER;
	case SHADER_VERTEX:          return GL_VERTEX_SHADER;
	default:                     return 0;
	}
}
}
}