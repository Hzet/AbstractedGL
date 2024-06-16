#include "agl/core/window.hpp"

namespace agl
{
namespace glfw
{
extern void error_callback(int error, const char* description);
static void window_button_input_callback(GLFWwindow* window, int button, int action, int mods);
static void window_char_callback(GLFWwindow* window, unsigned int codepoint);
static void window_close_callback(GLFWwindow* window);
static void window_cursor_enter_callback(GLFWwindow* window, int entered);
static void window_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
static void window_focus_callback(GLFWwindow* window, int focused);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void window_iconify_callback(GLFWwindow* window, int iconified);
static void window_key_input_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void window_maximize_callback(GLFWwindow* window, int maximnized);
static void window_scale_callback(GLFWwindow* window, float xscale, float yscale);
static void window_scroll_input_callback(GLFWwindow* window, double xoffset, double yoffset);
static void window_size_callback(GLFWwindow* window, int width, int height);

window::window()
	: m_clear_type{ CLEAR_COLOR }
	, m_handle{ nullptr }
	, m_is_focused{ false }
	, m_is_maximized{ false }
	, m_is_minimized{ false }
	, m_is_open{ false }
	, m_should_close{ false }
{
}
window::window(window&& other)
	: m_api_version{ std::move(other.m_api_version) }
	, m_clear_color{ other.m_clear_color }
	, m_frame_buffer_size{ other.m_frame_buffer_size }
	, m_handle{ other.m_handle }
	, m_is_focused{ other.m_is_focused }
	, m_is_maximized{ other.m_is_maximized }
	, m_is_minimized{ other.m_is_minimized }
	, m_is_open{ other.m_is_open }
	, m_resolution{ other.m_resolution }
	, m_title{ std::move(other.m_title) }
	, m_should_close{ other.m_should_close }
	, m_shading_language_version{ std::move(other.m_shading_language_version) }
{
	other.m_handle = nullptr;
}
window& window::operator=(window&& other)
{
	m_api_version = std::move(other.m_api_version);
	m_clear_color = other.m_clear_color;
	m_frame_buffer_size = other.m_frame_buffer_size;
	m_handle = other.m_handle;
	other.m_handle = nullptr;
	m_is_focused = other.m_is_focused;
	m_is_maximized = other.m_is_maximized;
	m_is_minimized = other.m_is_minimized;
	m_is_open = other.m_is_open;
	m_resolution = other.m_resolution;
	m_title = std::move(other.m_title);
	m_should_close = other.m_should_close;
	m_shading_language_version = std::move(other.m_shading_language_version);
	return *this;
}

window::~window()
{
	close();
}
void window::create(glm::uvec2 resolution, std::string const& title)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_title = title;
	m_resolution = resolution;
	m_is_focused = true;
	m_is_open = true;
	m_frame_buffer_size = m_resolution;
	m_handle = glfwCreateWindow(resolution.x, resolution.y, title.c_str(), nullptr, nullptr);

	if (m_handle == nullptr)
		throw std::exception{ "could not create a GLFW window" };

	glfwMakeContextCurrent(m_handle);
	set_callbacks();
}
glm::uvec2 const& window::size() const
{
	return m_resolution;
}
void window::resize(glm::uvec2 const& size)
{
	m_resolution = size;
}
GLFWwindow* window::get_handle()
{
	return m_handle;
}
bool window::is_minimized() const
{
	return m_is_minimized;
}
bool window::is_maximized() const
{
	return m_is_maximized;
}
bool window::is_focused() const
{
	return m_is_focused;
}
bool window::is_open() const
{
	return m_is_open;
}
std::string const& window::get_api_version() const
{
	return m_api_version;
}
std::string const& window::get_shading_language_version() const
{
	return m_shading_language_version;
}
void window::set_clear_type(clear_type type)
{
	m_clear_type = type;
}
clear_type window::get_clear_type() const
{
	return m_clear_type;
}
void window::set_clear_color(glm::vec4 const& color)
{
	m_clear_color = color;
}
glm::vec4 const& window::get_clear_color() const
{
	return m_clear_color;
}
void window::set_version(std::string const& api_version, std::string const& shading_language_version)
{
	m_api_version = api_version;
	m_shading_language_version = shading_language_version;
}
std::string const& window::title() const
{
	return m_title;
}
glm::uvec2 window::frame_buffer_size() const
{
	return m_frame_buffer_size;
}
void window::close()
{
	if (m_handle == nullptr)
		return;

	glfwDestroyWindow(m_handle);
	m_handle = nullptr;
	m_is_open = false;
}
bool window::poll_event(agl::event& event)
{
	if (m_event_queue.empty())
		return false;

	event = m_event_queue.front();
	m_event_queue.pop_front();
	return true;
}
void window::push_event(event e)
{
	m_event_queue.push_back(e);
}
void window::set_callbacks()
{
	glfwSetCharCallback(get_handle(), window_char_callback);
	glfwSetCursorEnterCallback(get_handle(), window_cursor_enter_callback);
	glfwSetErrorCallback(error_callback);
	glfwSetFramebufferSizeCallback(get_handle(), framebuffer_size_callback);
	glfwSetScrollCallback(get_handle(), window_scroll_input_callback);
	glfwSetWindowCloseCallback(get_handle(), window_close_callback);
	glfwSetWindowIconifyCallback(get_handle(), window_iconify_callback);
	glfwSetWindowFocusCallback(get_handle(), window_focus_callback);
	glfwSetWindowSizeCallback(get_handle(), window_size_callback);
	glfwSetWindowUserPointer(get_handle(), reinterpret_cast<void*>(this));
	glfwSetWindowMaximizeCallback(get_handle(), window_maximize_callback);
	glfwSetWindowContentScaleCallback(get_handle(), window_scale_callback);
	glfwSetKeyCallback(get_handle(), window_key_input_callback);
	glfwSetCursorPosCallback(get_handle(), window_cursor_position_callback);
	glfwSetMouseButtonCallback(get_handle(), window_button_input_callback);
}
bool window::should_close() const
{
	return m_should_close;
}

// callbacks
// ---------

void window_button_input_callback(GLFWwindow* glfw_handle, int button, int action, int mods)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto e = event{ GLFW_PRESS ? BUTTON_PRESSED : BUTTON_RELEASED };
	e.button.code = static_cast<button_type>(button);
	e.button.bit_modifiers = mods;
	wnd->push_event(e);
}

void window_char_callback(GLFWwindow* glfw_handle, unsigned int codepoint)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto e = event{ TEXT_ENTERED };
	e.character = codepoint;
	wnd->push_event(e);
}

void window_close_callback(GLFWwindow* glfw_handle)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_should_close = true;
	auto e = event{ WINDOW_CLOSED };
	wnd->push_event(e);
}

void window_cursor_enter_callback(GLFWwindow* glfw_handle, int entered)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto e = event{ entered ? MOUSE_ENTERED : MOUSE_LEFT };
	wnd->push_event(e);
}

void window_cursor_position_callback(GLFWwindow* glfw_handle, double xpos, double ypos)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto e = event{ MOUSE_MOVED };
	e.position = { static_cast<float>(xpos), static_cast<float>(ypos) };
	wnd->push_event(e);
}

void window_focus_callback(GLFWwindow* glfw_handle, int focused)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_is_focused = focused;
	auto e = event{ focused ? WINDOW_GAINED_FOCUS : WINDOW_LOST_FOCUS };
	wnd->push_event(e);
}

void framebuffer_size_callback(GLFWwindow* glfw_handle, int width, int height)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto e = event{ FRAMEBUFFER_RESIZED };
	wnd->m_frame_buffer_size.x = width;
	wnd->m_frame_buffer_size.y = height;
	wnd->push_event(e);
}

void window_iconify_callback(GLFWwindow* glfw_handle, int iconified)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto e = event{ iconified ? WINDOW_MINIMIZED : WINDOW_RESTORED };
	wnd->m_is_minimized = iconified;
	wnd->push_event(e);
}

void window_key_input_callback(GLFWwindow* glfw_handle, int key, int scancode, int action, int mods)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));

	auto e = event{};
	switch (action)
	{
	case GLFW_PRESS: e.type = KEY_PRESSED; break;
	case GLFW_RELEASE: e.type = KEY_RELEASED; break;
	case GLFW_REPEAT: e.type = KEY_REPEATED; break;
	}

	e.key.code = static_cast<key_type>(key);
	e.key.scancode = scancode;
	e.key.modifiers = mods;
	wnd->push_event(e);
}

void window_maximize_callback(GLFWwindow* glfw_handle, int maximized)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_is_minimized = false;
	wnd->m_is_maximized = maximized;
	auto e = event{ maximized ? WINDOW_MAXIMIZED : WINDOW_RESTORED };
	wnd->push_event(e);
}

void window_scale_callback(GLFWwindow* glfw_handle, float xscale, float yscale)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_is_maximized = false;

	if (xscale == 0.f && yscale == 0.f)
		wnd->m_is_minimized = true;

	auto e = event{ WINDOW_RESCALED };
	e.scale = { xscale, yscale };
	wnd->push_event(e);
}

void window_scroll_input_callback(GLFWwindow* glfw_handle, double xoffset, double yoffset)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));

	auto e = event{ MOUSE_SCROLL_MOVED };
	e.scroll = { static_cast<float>(xoffset), static_cast<float>(yoffset) };
	wnd->push_event(e);
}

void window_size_callback(GLFWwindow* glfw_handle, int width, int height)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_is_maximized = false;

	if (width == 0 && height == 0)
		wnd->m_is_minimized = true;

	auto e = event{ WINDOW_RESIZED };
	e.size = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
	wnd->push_event(e);
}
}
}