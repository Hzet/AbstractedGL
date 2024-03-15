#include "agl/core/events.hpp"
#include "agl/core/threads.hpp"
#include "agl/core/logger.hpp"
#include "agl/render/window.hpp"
#include <GLFW/glfw3.h>

namespace agl
{
namespace glfw
{
static events* g_events = nullptr;
static logger* g_logger = nullptr;

static constexpr const char* error_to_string(int code);
static void error_callback(int error, const char* description);
//static void set_callbacks(GLFWwindow* wnd);
//static void window_button_input_callback(GLFWwindow *window, int button, int action, int mods);
static void window_char_callback(GLFWwindow* window, unsigned int codepoint);
static void window_close_callback(GLFWwindow* window);
static void window_cursor_enter_callback(GLFWwindow* window, int entered);
//static void window_cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
static void window_focus_callback(GLFWwindow* window, int focused);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void window_iconify_callback(GLFWwindow* window, int iconified);
//static void window_key_input_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void window_maximize_callback(GLFWwindow* window, int maximnized);
static void window_scale_callback(GLFWwindow* window, float xscale, float yscale);
static void window_scroll_input_callback(GLFWwindow* window, double xoffset, double yoffset);
static void window_size_callback(GLFWwindow* window, int width, int height);
}
events::events()
	: resource<events>{}
{
	for (auto button : button_types)
		m_button_pressed.emplace({ button, false });
	
	for (auto key : key_types)
		m_key_pressed.emplace({ key, false });
}
void events::push_event(event e) noexcept
{
	std::lock_guard<std::mutex> lock{ *m_mutex };
	m_event_queue.push_back(e);
}
void events::set_button_pressed(button_type button, bool status) noexcept
{
	m_button_pressed.at(button) = status;
}
void events::set_key_pressed(key_type key, bool status) noexcept
{
	m_key_pressed.at(key) = status;
}
bool events::is_button_pressed(button_type button) const noexcept
{
	return m_button_pressed.at(button);
}
bool events::is_key_pressed(key_type key) const noexcept
{
	return m_key_pressed.at(key);
}
event events::poll_event() noexcept
{
	std::lock_guard<std::mutex> lock{ *m_mutex };
	auto e = m_event_queue.front();
	m_event_queue.pop_front();
	return e;
}
void events::on_attach(application* app)
{
	auto& logger = app->get_resource<agl::logger>();
	glfw::g_events = this;
	glfw::g_logger = &logger;
	m_mutex = make_unique<std::mutex>();
	
	logger.info("GLFW: Initializing");

	if (!glfwInit())
	{
		const char* description;
		int code = glfwGetError(&description);
		auto str = std::string{};
		if (description != nullptr)
			str = description;

		logger.error("GLFW: {}", str);
		throw std::exception{ logger::combine_message("Failed to initialize GLFW: {}", str).c_str() };
	}
	logger.info("GLFW: OK");
}
void events::set_window_callbacks(window* window)
{
	using namespace glfw;
	glfwSetCharCallback(window->get_handle(), window_char_callback);
	glfwSetCursorEnterCallback(window->get_handle(), window_cursor_enter_callback);
	glfwSetErrorCallback(error_callback);
	glfwSetFramebufferSizeCallback(window->get_handle(), framebuffer_size_callback);
	glfwSetScrollCallback(window->get_handle(), window_scroll_input_callback);
	glfwSetWindowCloseCallback(window->get_handle(), window_close_callback);
	glfwSetWindowIconifyCallback(window->get_handle(), window_iconify_callback);
	glfwSetWindowFocusCallback(window->get_handle(), window_focus_callback);
	glfwSetWindowSizeCallback(window->get_handle(), window_size_callback);
	glfwSetWindowUserPointer(window->get_handle(), reinterpret_cast<void*>(window));

	//glfwSetWindowMaximizeCallback(handle, glfw_window_maximize_callback);
	//glfwSetWindowContentScaleCallback(handle, glfw_window_scale_callback);
	//glfwSetKeyCallback(handle, glfw_window_key_input_callback);
	//glfwSetCursorPosCallback(handle, glfw_window_cursor_position_callback);
	//glfwSetMouseButtonCallback(handle, glfw_window_button_input_callback);
}
void events::on_detach(application* app)
{
	glfwTerminate();
	glfw::g_events = nullptr;
	glfw::g_logger = nullptr;

	auto& logger = app->get_resource<agl::logger>();
	logger.info("GLFW: OFF");
}
void events::on_update(application* app) noexcept
{
}
const vector<button_type> events::button_types = {
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_MID,
	BUTTON_4,
	BUTTON_5,
	BUTTON_6,
	BUTTON_7,
	BUTTON_8,
};
const vector<key_type> events::key_types = {
	SPACE,
	APOSTROPHE,
	COMMA,
	MINUS,
	PERIOD,
	SLASH,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	SEMICOLON,
	EQUAL,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	LEFT_BRACKET,
	BACKSLASH,
	RIGHT_BRACKET,
	GRAVE_ACCENT,
	WORLD_1,
	WORLD_2,
	ESCAPE,
	ENTER,
	TAB,
	BACKSPACE,
	INSERT,
	DELETE,
	RIGHT,
	LEFT,
	DOWN,
	UP,
	PAGE_UP,
	PAGE_DOWN,
	HOME,
	END,
	CAPS_LOCK,
	SCROLL_LOCK,
	NUM_LOCK,
	PRINT_SCREEN,
	PAUSE,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	F16,
	F17,
	F18,
	F19,
	F20,
	F21,
	F22,
	F23,
	F24,
	F25,
	KP_0,
	KP_1,
	KP_2,
	KP_3,
	KP_4,
	KP_5,
	KP_6,
	KP_7,
	KP_8,
	KP_9,
	KP_DECIMAL,
	KP_DIVIDE,
	KP_MULTIPLY,
	KP_SUBTRACT,
	KP_ADD,
	KP_ENTER,
	KP_EQUAL,
	LEFT_SHIFT,
	LEFT_CONTROL,
	LEFT_ALT,
	LEFT_SUPER,
	RIGHT_SHIFT,
	RIGHT_CONTROL,
	RIGHT_ALT,
	RIGHT_SUPER,
	MENU,
};

namespace glfw
{
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

void error_callback(int code, const char* description)
{
	AGL_ASSERT(g_logger != nullptr, "invalid logger pointer");

	g_logger->error("GLFW: {}\n{}", error_to_string(code), description);
}

constexpr const char* error_to_string(int code)
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
	AGL_ASSERT(false, "invalid glfw error code");
	return "GLFW_UNKNOWN_ERROR";
}


void window_char_callback(GLFWwindow* glfw_handle, unsigned int codepoint)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto e = event{ TEXT_ENTERED, wnd };
	e.character = codepoint;
	g_events->push_event(e);
}

void window_close_callback(GLFWwindow* glfw_handle)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_should_close = true;
	auto e = event{ WINDOW_CLOSED, wnd };
	g_events->push_event(e);
}

void window_cursor_enter_callback(GLFWwindow* glfw_handle, int entered)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto e = event{ entered ? MOUSE_ENTERED : MOUSE_LEFT, wnd };
	g_events->push_event(e);
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

void window_focus_callback(GLFWwindow* glfw_handle, int focused)
{
	auto* wnd= reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_is_focused = focused;
	auto e = event{ focused ? WINDOW_GAINED_FOCUS : WINDOW_LOST_FOCUS, wnd };
	g_events->push_event(e);
}

void framebuffer_size_callback(GLFWwindow* glfw_handle, int width, int height)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_frame_buffer_size.x = width;
	wnd->m_frame_buffer_size.y = height;
	wnd->resize({ width, height });
}

void window_iconify_callback(GLFWwindow* glfw_handle, int iconified)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));

	auto e = event{ iconified ? WINDOW_MINIMIZED : WINDOW_RESTORED, wnd };
	wnd->m_is_minimized = iconified;
	g_events->push_event(e);
}

/*void window_key_input_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
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

void window_maximize_callback(GLFWwindow* glfw_handle, int maximized)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_is_minimized = false;
	wnd->m_is_maximized = maximized;
	auto e = event{ maximized ? WINDOW_MAXIMIZED : WINDOW_RESTORED, wnd };
	g_events->push_event(e);
}

void window_scale_callback(GLFWwindow* glfw_handle, float xscale, float yscale)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_is_maximized = false;

	if (xscale == 0.f && yscale == 0.f)
		wnd->m_is_minimized = true;

	auto e = event{ WINDOW_RESCALED, wnd };
	e.scale = { xscale, yscale };
	g_events->push_event(e);
}

void window_scroll_input_callback(GLFWwindow* glfw_handle, double xoffset, double yoffset)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));

	auto e = event{ MOUSE_SCROLL_MOVED, wnd };
	e.scroll = { static_cast<float>(xoffset), static_cast<float>(yoffset) };
	g_events->push_event(e);
}

void window_size_callback(GLFWwindow* glfw_handle, int width, int height)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	wnd->m_is_maximized = false;

	if (width == 0 && height == 0)
		wnd->m_is_minimized = true;

	auto e = event{ WINDOW_RESIZED, wnd};
	e.size = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
	g_events->push_event(e);
}
}
}