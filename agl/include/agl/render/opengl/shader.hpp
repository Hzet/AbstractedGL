#pragma once
#include "agl/render/opengl/call.hpp"
#include "agl/render/shader.hpp"
#include "agl/vector.hpp"

namespace agl
{
namespace opengl
{
enum shader_type
{
	SHADER_INVALID = 0,
	SHADER_VERTEX = GL_VERTEX_SHADER,
	SHADER_TESS_CONTROL = GL_TESS_CONTROL_SHADER,
	SHADER_TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
	SHADER_GEOMETRY = GL_GEOMETRY_SHADER,
	SHADER_FRAGMENT = GL_FRAGMENT_SHADER,
	SHADER_COMPUTE = GL_COMPUTE_SHADER,
};

class shader
	: public agl::shader
{
public:
	shader();
	shader(shader&&);
	shader& operator=(shader&&);
	~shader();

	void destroy();
	void link();
	virtual void load_from_file(std::string const& filepath) override;

private:
	struct sub_shader
	{
		std::uint64_t descriptor;
		shader_type type;
	};

private:
	void destroy_sub_shaders();
	void load_sub_shader(shader_type type, std::string const& source);

private:
	std::uint64_t m_descriptor;
	std::string m_filepath;
	vector<sub_shader> m_sub_shaders;
};
}
}