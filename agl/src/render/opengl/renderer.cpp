#include <GLFW/glfw3.h>
#include "agl/core/events.hpp"
#include "agl/core/logger.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/render/opengl/call.hpp"
#include "agl/render/opengl/renderer.hpp"
#include "agl/render/opengl/feature-type.hpp"
#include "agl/render/opengl/window.hpp"

namespace agl
{
namespace opengl
{
static constexpr const char* glfw_error_to_string(int code);
static void glfw_error_callback(int error, const char* description);
static void glfw_set_callbacks(GLFWwindow* wnd);
//static void glfw_window_button_input_callback(GLFWwindow *window, int button, int action, int mods);
static void glfw_window_char_callback(GLFWwindow * window, unsigned int codepoint);
static void glfw_window_close_callback(GLFWwindow * window);
static void glfw_window_cursor_enter_callback(GLFWwindow * window, int entered);
//static void glfw_window_cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
static void glfw_window_focus_callback(GLFWwindow * window, int focused);
static void glfw_framebuffer_size_callback(GLFWwindow * window, int width, int height);
static void glfw_window_iconify_callback(GLFWwindow * window, int iconified);
//static void glfw_window_key_input_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void glfw_window_maximize_callback(GLFWwindow * window, int maximnized);
static void glfw_window_scale_callback(GLFWwindow * window, float xscale, float yscale);
static void glfw_window_scroll_input_callback(GLFWwindow * window, double xoffset, double yoffset);
static void glfw_window_size_callback(GLFWwindow * window, int width, int height);

#ifdef AGL_DEBUG
static void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif

logger* renderer::m_logger = nullptr;

window* renderer::create_window(std::string const& title, glm::uvec2 const& resolution, window::update_fun fun)
{
	auto* handle = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr, nullptr);

	if (handle == nullptr)
		throw std::exception{ "could not open window" };

	auto& ecs = m_app->get_resource<ecs::organizer>();
	auto wnd_ent = ecs.make_entity();
	{
		auto wnd = window{ handle, title, resolution, fun };
		ecs.push_component<window>(wnd_ent, std::move(wnd));
	}
	auto& wnd = wnd_ent.get_component<window>(0);
	make_window_current(&wnd);

	// initialize GLAD library
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::exception{ "Failed to initialize OpenGL context!" };

	auto& events = m_app->get_resource<agl::events>();
	auto& logger = m_app->get_resource<agl::logger>();
	wnd.m_data = { &events, &logger, &wnd };

	glfwSetWindowUserPointer(wnd.get_handle(), reinterpret_cast<void*>(&wnd.m_data));
	glfw_set_callbacks(wnd.get_handle());

#ifdef AGL_DEBUG
	wnd.enable_feature(FEATURE_DEBUG_OUTPUT);
	wnd.enable_feature(FEATURE_DEBUG_OUTPUT_SYNCHRONOUS);

	AGL_OPENGL_CALL(glDebugMessageCallback(gl_debug_callback, reinterpret_cast<void*>(&wnd.m_data)));
	AGL_OPENGL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE));
#endif
}

void renderer::on_attach(application* app) noexcept
{
	auto& log = app->get_resource<logger>();
	log.info("OpenGL renderer: Initializing");
	log.info("GLFW: Initializing");

	if (!glfwInit())
	{
		const char* description;
		int code = glfwGetError(&description);
		auto str = std::string{};
		if (description != nullptr)
			str = description;

		app->get_resource<logger>().error("GLFW: {}", str);
	}
	log.info("GLFW: OK");
}

// render
void renderer::on_update(application*) noexcept 
{

}

// unload opengl
void renderer::on_detach(application* app) noexcept 
{
	auto& log = app->get_resource<logger>();
	
	glfwTerminate();
	log.info("GLFW: OFF");

	log.info("OpenGL renderer: OFF");

}
void renderer::make_window_current(window* wnd) noexcept
{
	m_current_window = wnd;
}

// -------------
// - CALLBACKS -
// -------------

void glfw_set_callbacks(GLFWwindow* handle)
{

	glfwSetCharCallback(handle, glfw_window_char_callback);
	glfwSetCursorEnterCallback(handle, glfw_window_cursor_enter_callback);
	glfwSetErrorCallback(glfw_error_callback);
	glfwSetFramebufferSizeCallback(handle, glfw_framebuffer_size_callback);
	glfwSetScrollCallback(handle, glfw_window_scroll_input_callback);
	glfwSetWindowCloseCallback(handle, glfw_window_close_callback);
	glfwSetWindowIconifyCallback(handle, glfw_window_iconify_callback);
	glfwSetWindowFocusCallback(handle, glfw_window_focus_callback);
	glfwSetWindowSizeCallback(handle, glfw_window_size_callback);
	//glfwSetWindowMaximizeCallback(handle, glfw_window_maximize_callback);
	//glfwSetWindowContentScaleCallback(handle, glfw_window_scale_callback);
	//glfwSetKeyCallback(handle, glfw_window_key_input_callback);
	//glfwSetCursorPosCallback(handle, glfw_window_cursor_position_callback);
	//glfwSetMouseButtonCallback(handle, glfw_window_button_input_callback);
}

/*
	void glfw_window_button_input_callback(GLFWwindow *window, int button, int action, int mods)
	{
		auto &data = *reinterpret_cast<window_data*>(glfwGetWindowUserPointer(window));

		auto e = event{};
		switch (action)
		{
		case GLFW_PRESS:
			e.type = MOUSE_KEY_PRESSED;
			break;
		case GLFW_RELEASE:
			e.type = MOUSE_KEY_RELEASED;
			break;
		}

		e.button.code = static_cast<button_type>(button);
		e.button.bit_modifiers = mods;

		data.events.push(e);
	}*/

void glfw_error_callback(int code, const char* description)
{
	//TODO: logger here
	renderer::m_logger->error("GLFW: {}\n{}", glfw_error_to_string(code), description);
}

constexpr const char* glfw_error_to_string(int code)
{
	
	switch (code)
	{
	case GLFW_NO_CURRENT_CONTEXT: return "GLFW_NO_CURRENT_CONTEXT";
	case GLFW_INVALID_ENUM: return "GLFW_INVALID_ENUM";
	case GLFW_INVALID_VALUE: return "GLFW_INVALID_VALUE";
	case GLFW_OUT_OF_MEMORY: return "GLFW_OUT_OF_MEMORY";
	case GLFW_API_UNAVAILABLE: return "GLFW_API_UNAVAILABLE";
	case GLFW_VERSION_UNAVAILABLE: return "GLFW_VERSION_UNAVAILABLE";
	case GLFW_PLATFORM_ERROR: return "GLFW_PLATFORM_ERROR";
	case GLFW_FORMAT_UNAVAILABLE: return "GLFW_FORMAT_UNAVAILABLE";
	}
}


void glfw_window_char_callback(GLFWwindow* window, unsigned int codepoint)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));

	auto e = event{};
	e.type = TEXT_ENTERED;
	e.character = codepoint;
	e.wnd = data->window;
	data->events->push_event(e);
}

void glfw_window_close_callback(GLFWwindow* window)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));
	data->window->m_properties.running = false;

	auto e = event{};
	e.type = WINDOW_CLOSED;
	e.wnd = data->window;
	data->events->push_event(e);
}

void glfw_window_cursor_enter_callback(GLFWwindow* window, int entered)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));

	auto e = event{};
	e.type = entered ? MOUSE_ENTERED : MOUSE_LEFT;
	data->events->push_event(e);
}

/*
	void glfw_window_cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
	{
		auto&data = *reinterpret_cast<window_data*>(glfwGetWindowUserPointer(window));

		auto e = event{};
		e.type = MOUSE_MOVED;
		e.position = { static_cast<float>(xpos), static_cast<float>(ypos) };

		data.events.push(e);
	}*/

void glfw_window_focus_callback(GLFWwindow* window, int focused)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));
	data->window->m_properties.focused = focused;

	auto e = event{};
	e.type = focused ? WINDOW_GAINED_FOCUS : WINDOW_LOST_FOCUS;
	e.wnd = data->window;
	data->events->push(e);
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));
	data->window->m_properties.frame_buffer_size.x = width;
	data->window->m_properties.frame_buffer_size.y = height;

	AGL_OPENGL_CALL(glViewport(0, 0, width, height));
}

void glfw_window_iconify_callback(GLFWwindow* window, int iconified)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));

	auto e = event{};
	e.type = iconified ? WINDOW_MINIMIZED : WINDOW_RESTORED;
	e.wnd = data->window;
	data->window->m_properties.minimized = iconified;
	data->events->push(e);
}

/*void glfw_window_key_input_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	auto& data = *reinterpret_cast<window_data*>(glfwGetWindowUserPointer(window));

	auto e = event{};
	switch (action)
	{
	case GLFW_PRESS:
		e.type = KEY_PRESSED;
		break;
	case GLFW_RELEASE:
		e.type = KEY_RELEASED;
		break;
	case GLFW_REPEAT:
		e.type = KEY_REPEATED;
		break;
	}

	e.key.code = static_cast<Keyboard::key_type>(key);
	e.key.scancode = scancode;
	e.key.modifiers = mods;

	data.events.push(e);
}*/

void glfw_window_maximize_callback(GLFWwindow* window, int maximized)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));
	data->window->m_properties.minimized = false;
	data->window->m_properties.maximized = maximized;

	auto e = event{};
	e.type = maximized ? WINDOW_MAXIMIZED : WINDOW_RESTORED;
	e.wnd = data->window;
	data->events->push(e);
}

void glfw_window_scale_callback(GLFWwindow* window, float xscale, float yscale)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));
	data->window->m_properties.maximized = false;

	if (xscale == 0.f && yscale == 0.f)
		data->window->m_properties.minimized = true;

	auto e = event{};
	e.type = WINDOW_RESCALED;
	e.scale = { xscale, yscale };
	e.wnd = data->window;
	data->events->push(e);
}

void glfw_window_scroll_input_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));

	auto e = event{};
	e.type = MOUSE_SCROLL_MOVED;
	e.scroll = { static_cast<float>(xoffset), static_cast<float>(yoffset) };
	e.wnd = data->window;
	data->events->push(e);
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
	auto* data = reinterpret_cast<window::data*>(glfwGetWindowUserPointer(window));
	data->window->m_properties.maximized = false;

	if (width == 0 && height == 0)
		data->window->m_properties.minimized = true;

	auto e = event{};
	e.type = WINDOW_RESIZED;
	e.size = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
	e.wnd = data->window;
	data->events->push(e);
}

#ifdef AGL_DEBUG
void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	auto const* data = reinterpret_cast<window::data const*>(userParam);
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         data->logger->error(message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:       data->logger->warning(message); return;
	case GL_DEBUG_SEVERITY_LOW:          data->logger->warning(message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: data->logger->trace(message); return;
	default: break;
	}
}
#endif
}
}