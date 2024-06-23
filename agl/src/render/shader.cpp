#include "agl/render/shader.hpp"

namespace agl
{
shader::shader(std::string const& filepath, std::uint64_t id)
	: m_filepath{ filepath }
	, m_id{ id }
{
}
std::string const& shader::get_filepath() const
{
	return m_filepath;
}
std::uint64_t shader::get_id() const
{
	return m_id;
}
const char* get_shader_type_name(shader_type type)
{
	switch (type)
	{
	case SHADER_COMPUTE:         return "compute";
	case SHADER_FRAGMENT:        return "fragment";
	case SHADER_GEOMETRY:        return "geometry";
	case SHADER_TESS_CONTROL:    return "tess_control";
	case SHADER_TESS_EVALUATION: return "tess_evaluation";
	case SHADER_VERTEX:          return "vertex";
	default:                     return "invalid";
	}
}
}