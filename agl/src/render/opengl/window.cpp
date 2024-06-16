#include "agl/render/opengl/call.hpp"
#include "agl/render/opengl/window.hpp"
#include "agl/core/events.hpp"
#include "agl/core/logger.hpp"

namespace agl
{
namespace opengl
{
static constexpr std::uint32_t get_opengl_feature_code(feature_type feature);

void window::create(glm::uvec2 resolution, std::string const& title)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfw::window::create(resolution, title);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::exception{ "Failed to initialize OpenGL context!" };

	auto gl_version = std::string{};
	auto glsl_version = std::string{};
	AGL_OPENGL_CALL(gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	AGL_OPENGL_CALL(glsl_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
	gl_version = logger::combine_message("OpenGL: {}", gl_version);
	gl_version = logger::combine_message("GLSL: {}", glsl_version);
	set_version(gl_version, glsl_version);
}
void window::feature_disable(feature_type feature)
{
	AGL_OPENGL_CALL(glDisable(get_opengl_feature_code(feature)));
}
void window::feature_enable(feature_type feature)
{
	AGL_OPENGL_CALL(glEnable(get_opengl_feature_code(feature)));
}
bool window::feature_status(feature_type feature)
{
	auto status = bool{};
	AGL_OPENGL_CALL(status = glIsEnabled(get_opengl_feature_code(feature)));
	return status;
}
static constexpr std::uint32_t get_opengl_feature_code(feature_type feature)
{
	switch (feature)
	{
	case FEATURE_BLEND: return GL_BLEND;
	case FEATURE_COLOR_LOGIC_OP: return GL_COLOR_LOGIC_OP;
	case FEATURE_CULL_FACE: return GL_CULL_FACE;
	case FEATURE_DEBUG_OUTPUT: return GL_DEBUG_OUTPUT;
	case FEATURE_DEBUG_OUTPUT_SYNCHRONOUS: return GL_DEBUG_OUTPUT_SYNCHRONOUS;
	case FEATURE_DEPTH_CLAMP: return GL_DEPTH_CLAMP;
	case FEATURE_DEPTH_TEST: return GL_DEPTH_TEST;
	case FEATURE_DITHER: return GL_DITHER;
	case FEATURE_FRAMEBUFFER_SRGB: return GL_FRAMEBUFFER_SRGB;
	case FEATURE_LINE_SMOOTH: return GL_LINE_SMOOTH;
	case FEATURE_MULTISAMPLE: return GL_MULTISAMPLE;
	case FEATURE_POLYGON_OFFSET_FILL: return GL_POLYGON_OFFSET_FILL;
	case FEATURE_POLYGON_OFFSET_LINE: return GL_POLYGON_OFFSET_LINE;
	case FEATURE_POLYGON_OFFSET_POINT: return GL_POLYGON_OFFSET_POINT;
	case FEATURE_POLYGON_SMOOTH: return GL_POLYGON_SMOOTH;
	case FEATURE_PRIMITIVE_RESTART: return GL_PRIMITIVE_RESTART;
	case FEATURE_PRIMITIVE_RESTART_FIXED_INDEX: return GL_PRIMITIVE_RESTART_FIXED_INDEX;
	case FEATURE_RASTERIZER_DISCARD: return GL_RASTERIZER_DISCARD;
	case FEATURE_SAMPLE_ALPHA_TO_COVERAGE: return GL_SAMPLE_ALPHA_TO_COVERAGE;
	case FEATURE_SAMPLE_ALPHA_TO_ONE: return GL_SAMPLE_ALPHA_TO_ONE;
	case FEATURE_SAMPLE_COVERAGE: return GL_SAMPLE_COVERAGE;
	case FEATURE_SAMPLE_SHADING: return GL_SAMPLE_SHADING;
	case FEATURE_SAMPLE_MASK: return GL_SAMPLE_MASK;
	case FEATURE_SCISSOR_TEST: return GL_SCISSOR_TEST;
	case FEATURE_STENCIL_TEST: return GL_STENCIL_TEST;
	case FEATURE_TEXTURE_CUBE_MAP_SEAMLESS: return GL_TEXTURE_CUBE_MAP_SEAMLESS;
	case FEATURE_PROGRAM_POINT_SIZE: return GL_PROGRAM_POINT_SIZE;
	}
	AGL_ASSERT(false, "invalid feature type");
	return 0;
}
}
}