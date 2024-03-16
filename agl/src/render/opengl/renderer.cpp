#include "agl/render/opengl/renderer.hpp"
#include "agl/render/opengl/window.hpp"
#include "agl/core/logger.hpp"
#include "agl/core/events.hpp"
#include "agl/ecs/ecs.hpp"

namespace agl
{
namespace opengl
{
#ifdef AGL_DEBUG
static agl::logger* g_logger = nullptr;

static void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif

renderer::renderer()
	: agl::renderer{ ecs::RENDER }
{
}

renderer::renderer(renderer&& other)
	: agl::renderer{ std::move(other) }
{
}
renderer& renderer::operator=(renderer&& other)
{
	this->agl::renderer::operator=(std::move(other));
	return *this;
}
agl::window& renderer::create_window(glm::uvec2 const& resolution, std::string const& title)
{
	get_organizer().push_component<opengl::window>(m_windows);
	auto& window = m_windows.get_component<opengl::window>(m_windows.size<opengl::window>() - 1);
	window.hint_api_version(4, 3);
	window.create(resolution, title);

#ifdef AGL_DEBUG
	window.feature_enable(FEATURE_DEBUG_OUTPUT);
	window.feature_enable(FEATURE_DEBUG_OUTPUT_SYNCHRONOUS);

	AGL_OPENGL_CALL(glDebugMessageCallback(gl_debug_callback, nullptr));
	AGL_OPENGL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE));
	g_logger->debug("OpenGL debug messages: ON");
	g_logger->debug("New window: {}, {}", window.get_api_version(), window.get_shading_language_version());
#endif
	
	return window;
}
void renderer::on_attach(application* app)
{
	auto& logger = app->get_resource<agl::logger>();
	m_windows = get_organizer().make_entity();
	logger.info("OpenGL renderer: OK");

#ifdef AGL_DEBUG
	g_logger = &app->get_resource<agl::logger>();
#endif
}
// render
void renderer::on_update(application* app)
{
	auto size = m_windows.size<opengl::window>();
	for (auto i = 0; i < size; ++i)
	{
		auto& window = m_windows.get_component<opengl::window>(i);
		auto* handle = window.get_handle();
		glfwMakeContextCurrent(handle);
		
		if (!window.should_close())
		{
			glfwSwapBuffers(handle);
			glfwPollEvents();
		}
		else 
		{
			window.close();
			get_organizer().pop_component<opengl::window>(m_windows, i);
			--size;
		}
	}
}
// unload opengl
void renderer::on_detach(application* app)
{
	auto& logger = app->get_resource<agl::logger>();
	logger.info("OpenGL renderer: Exiting");
	get_organizer().destroy_entity(m_windows);
	logger.info("OpenGL renderer: OFF");

#ifdef AGL_DEBUG
	g_logger = nullptr;
#endif
}
agl::window& renderer::get_window(std::uint64_t index)
{
	return m_windows.get_component<window>(index);
}

#ifdef AGL_DEBUG
void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         g_logger->debug("OpenGL debug output: Severity [{}]. Message: {}.", "HIGH", message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:       g_logger->debug("OpenGL debug output: Severity [{}]. Message: {}.", "MEDIUM", message); return;
	case GL_DEBUG_SEVERITY_LOW:          g_logger->debug("OpenGL debug output: Severity [{}]. Message: {}.", "LOW", message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: g_logger->debug("OpenGL debug output: Severity [{}]. Message: {}.", "NOTIFICATION", message); return;
	default: break;
	}
}
#endif
}
}