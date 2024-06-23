#pragma once
#include <string>
#include "agl/dictionary.hpp"

namespace agl
{
enum shader_type
{
	SHADER_COMPUTE,
	SHADER_FRAGMENT,
	SHADER_GEOMETRY,
	SHADER_INVALID,
	SHADER_TESS_CONTROL,
	SHADER_TESS_EVALUATION,
	SHADER_VERTEX,
};


class shader
{
public:
                       shader(std::string const& filepath = "", std::uint64_t id = 0);
	std::string const& get_filepath() const;
	std::uint64_t      get_id() const;

private:
	std::string   m_filepath;
	std::uint64_t m_id;
};

const char* get_shader_type_name(shader_type type);
}