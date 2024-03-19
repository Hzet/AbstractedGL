#include "agl/render/opengl/call.hpp"
#include "agl/render/opengl/shader.hpp"
#include "agl/core/logger.hpp"
#include <regex>
#include <fstream>

namespace agl
{
namespace opengl
{
struct shader_source
{
	shader_type type;
	std::string source;
};
static std::string get_sub_shader_type_string(shader_type type)
{
	switch (type)
	{
	case SHADER_VERTEX: return "vertex";
	case SHADER_TESS_CONTROL: return "tess_control";
	case SHADER_TESS_EVALUATION: return "tess_evaluation";
	case SHADER_GEOMETRY: return "geometry";
	case SHADER_FRAGMENT: return "fragment";
	case SHADER_COMPUTE: return "compute";
	}
	AGL_ASSERT(false, "invalid shader type");
	return "INVALID_SHADER_TYPE";
}

static vector<shader_source> parse_shader_source(std::string const& source)
{
	struct keyword
	{
		std::uint64_t index;
		shader_type type;
	};

	auto keywords = std::vector<keyword>{
		{ std::string::npos, SHADER_VERTEX },
		{ std::string::npos, SHADER_TESS_CONTROL },
		{ std::string::npos, SHADER_TESS_EVALUATION },
		{ std::string::npos, SHADER_GEOMETRY },
		{ std::string::npos, SHADER_FRAGMENT },
		{ std::string::npos, SHADER_COMPUTE },
	};

	for (auto& k : keywords)
		k.index = source.find(get_sub_shader_type_string(k.type));

	std::sort(keywords.begin(), keywords.end(), [](auto const& lhs, auto const& rhs) { return lhs.index < rhs.index; });

	auto result = vector<shader_source>{};
	for (auto i = 0; i < keywords.size(); ++i)
	{
		if (keywords[i].index == std::string::npos)
			break;

		auto src = std::string{};
		if (i == keywords.size() - 1)
			src = source.substr(keywords[i].index);
		else
			src = source.substr(keywords[i].index, keywords[i + 1].index - keywords[i].index);
		src.erase(0, get_sub_shader_type_string(keywords[i].type).size() + 1);
		result.push_back({ keywords[i].type, src });
	}

	return result;
}

shader::shader()
	: m_descriptor{ 0 }
{
}
shader::shader(shader&& other)
	: m_descriptor{ other.m_descriptor }
	, m_filepath{ std::move(other.m_filepath) }
	, m_sub_shaders{ std::move(other.m_sub_shaders) }
{
	other.m_descriptor = 0;
}
shader& shader::operator=(shader&& other)
{
	if (this == &other)
		return *this;

	m_descriptor = other.m_descriptor;
	m_filepath = std::move(other.m_filepath);
	m_sub_shaders = std::move(other.m_sub_shaders);
	return *this;
}
shader::~shader()
{
	destroy();
}
void shader::link()
{
	destroy();

	AGL_OPENGL_CALL(m_descriptor = glCreateProgram());

	for (auto& sub_shader : m_sub_shaders)
		AGL_OPENGL_CALL(glAttachShader(m_descriptor, sub_shader.descriptor));

	AGL_OPENGL_CALL(glLinkProgram(m_descriptor));

	auto success = std::int32_t{};
	AGL_OPENGL_CALL(glGetProgramiv(m_descriptor, GL_LINK_STATUS, &success));

	m_sub_shaders.clear();
	if (!success)
	{
		auto length = std::int32_t{};
		AGL_OPENGL_CALL(glGetProgramiv(m_descriptor, GL_INFO_LOG_LENGTH, &length));

		auto message = std::string{};
		message.resize(length);

		AGL_OPENGL_CALL(glGetProgramInfoLog(m_descriptor, length, NULL, &message[0u]));
		throw std::exception{ logger::combine_message("Failed to link shader program: {}: \"{}\"", m_filepath, message).c_str() };
	}
}
void shader::destroy()
{
	if (m_descriptor == 0)
		return;
	AGL_OPENGL_CALL(glDeleteProgram(m_descriptor));
	m_descriptor = 0;
}
void shader::load_from_file(std::string const& filepath)
{
	auto file = std::ifstream{ filepath, std::ios::in };
	
	auto ss = std::stringstream{};
	file.seekg(0, std::ios::beg);
	ss << file.rdbuf();
	file.close();

	auto source = ss.str();
	auto sources = parse_shader_source(source);
	for (auto const& src : sources)
		load_sub_shader(src.type, src.source);
}
void shader::destroy_sub_shaders()
{
	for (auto& sub_shader : m_sub_shaders)
		AGL_OPENGL_CALL(glDeleteShader(sub_shader.descriptor));
}
void shader::load_sub_shader(shader_type type, std::string const& source)
{
	auto descriptor = std::uint32_t{};
	auto success = std::int32_t{};
	auto const* src = source.c_str();
	AGL_OPENGL_CALL(descriptor = glCreateShader(type));
	AGL_OPENGL_CALL(glShaderSource(descriptor, 1, &src, NULL));
	AGL_OPENGL_CALL(glCompileShader(descriptor));
	AGL_OPENGL_CALL(glGetShaderiv(descriptor, GL_COMPILE_STATUS, &success));

	if (!success)
	{
		auto length = std::int32_t{};
		auto message = std::string{};
		AGL_OPENGL_CALL(glGetShaderiv(descriptor, GL_INFO_LOG_LENGTH, &length));

		message.resize(length);
		AGL_OPENGL_CALL(glGetShaderInfoLog(descriptor, length, nullptr, &message[0u]));

		throw std::exception{ logger::combine_message("Failed to compile subshader {}: {}", get_sub_shader_type_string(type), message).c_str()};
	}

	m_sub_shaders.push_back(sub_shader{ descriptor, type });
}
}
}