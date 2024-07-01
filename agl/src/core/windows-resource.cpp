#include "agl/core/windows-resource.hpp"
#include "agl/core/logger.hpp"

namespace agl
{
#ifdef AGL_DEBUG
static void error_callback(int error, const char* description);
#endif
static void window_button_input_callback(GLFWwindow* window, int button, int action, int mods);
static void window_char_callback(GLFWwindow* window, unsigned int codepoint);
static void window_close_callback(GLFWwindow* window);
static void window_cursor_enter_callback(GLFWwindow* window, int entered);
static void window_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
static void window_focus_callback(GLFWwindow* window, int focused);
static void window_framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void window_iconify_callback(GLFWwindow* window, int iconified);
static void window_key_input_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void window_maximize_callback(GLFWwindow* window, int maximnized);
static void window_scale_callback(GLFWwindow* window, float xscale, float yscale);
static void window_scroll_input_callback(GLFWwindow* window, double xoffset, double yoffset);
static void window_size_callback(GLFWwindow* window, int width, int height);


static windows_resource* g_api = nullptr;
#ifdef AGL_DEBUG
static logger* g_logger = nullptr;
#endif


windows_resource::windows_resource()
	: resource<windows_resource>{}
	, windows_event_resource{}
{
	m_glfw_initialized = false;
	g_api = this;
}
windows_resource::~windows_resource() noexcept
{
	g_api = nullptr;
}
bool windows_resource::is_glfw_initialized() const
{
	return m_glfw_initialized;
}
void windows_resource::initialize()
{
	if (!glfwInit())
	{
		const char* description;
		int code = glfwGetError(&description);
		auto str = std::string{};
		if (description != nullptr)
			str = description;

		throw std::exception{ logger::combine_message("Failed to initialize GLFW: {}", str).c_str() };
	}
	m_glfw_initialized = true;
}
void windows_resource::deinitialize()
{
	if (!is_glfw_initialized())
		return;
	
	destroy_windows();
	glfwTerminate();
	m_glfw_initialized = false;
}
void windows_resource::on_attach(application* app)
{
	AGL_CORE_ASSERT(!is_glfw_initialized(), "glfw must be deinitialized upon reaching this method");
	auto* logger = app->get_resource<agl::logger>();
#ifdef AGL_DEBUG
	g_logger = logger;
#endif
	logger->debug("GLFW: Initializing");
	initialize();
	logger->debug("GLFW: OK");
}
void windows_resource::on_detach(application* app)
{
	deinitialize();

	m_glfw_initialized = false;
#ifdef AGL_DEBUG
	g_logger = nullptr;
#endif

	auto* logger = app->get_resource<agl::logger>();
	logger->debug("GLFW: OFF");
}
void windows_resource::on_update(application* app)
{
	clear_events();
	glfwPollEvents();
	process_events();
}
void windows_event_resource::set_current_context(window* wnd)
{
	AGL_CORE_ASSERT(wnd != nullptr, "invalid window pointer");
	glfwMakeContextCurrent(wnd->get_handle());
	m_current_context = wnd;
}
window* windows_event_resource::get_window(std::uint64_t index)
{
	return m_windows[index].get();
}
vector<unique_ptr<window>> const& windows_event_resource::get_windows() const
{
	return m_windows;
}
void windows_event_resource::clear_events()
{
	for (auto& wnd : m_windows)
		wnd->m_events.clear();
}
void windows_event_resource::process_events()
{
	for(auto& wnd : m_windows)
		for(auto const e : wnd->get_events())
			switch (e.get_type())
			{
			case WINDOW_SHOULD_CLOSE: window_should_close(e); break;
			case WINDOW_GAINED_FOCUS: window_gain_focus(e); break;
			case WINDOW_LOST_FOCUS:	  window_lose_focus(e); break;
			case WINDOW_MAXIMIZED:    window_maximize(e); break;
			case WINDOW_MINIMIZED:	  window_minimize(e); break;
			case WINDOW_RESTORED:     window_restore(e); break;
			default: break;
			}
}
void windows_event_resource::push_event(window* wnd, event e)
{
	wnd->m_events.push_back(e);
}
void windows_event_resource::destroy_window(window* wnd)
{
	if (wnd == nullptr || wnd->get_handle() == nullptr)
		return;

	auto found = std::find_if(m_windows.cbegin(), m_windows.cend(), [wnd](auto const& w) { return w == wnd; });
	AGL_CORE_ASSERT(found != m_windows.cend(), "window was not created by this api");

#ifdef AGL_DEBUG
	g_logger->debug("Window destroyed: {} \\{{}, {}\\}", wnd->get_title(), wnd->get_resolution().x, wnd->get_resolution().y);
#endif

	glfwDestroyWindow(wnd->get_handle());
	wnd->m_handle = nullptr;
	wnd->m_close_next_frame = false;
	wnd->m_is_focused = false;
	wnd->m_is_maximized = false;
	wnd->m_is_minimized = false;
	wnd->m_is_open = false;
	m_windows.erase(found);
	m_current_context = (m_windows.empty() ? nullptr : m_windows.back().get());
}
window* windows_event_resource::get_current_context()
{
	return m_current_context;
}
window const* windows_event_resource::get_current_context() const
{
	return m_current_context;
}
glm::uvec2 windows_event_resource::get_framebuffer_size(window* wnd)
{
	if (wnd == nullptr || wnd->get_handle() == nullptr)
		return {};

	auto ivec2 = glm::ivec2{};
	glfwGetFramebufferSize(wnd->get_handle(), &ivec2.x, &ivec2.y);
	return ivec2;
}
windows_event_resource::windows_event_resource()
	: m_current_context{ nullptr }
{
}
windows_event_resource::~windows_event_resource()
{
	destroy_windows();
}
void windows_event_resource::destroy_windows()
{
	for (auto& wnd : m_windows)
		destroy_window(wnd.get());
}
window* windows_event_resource::create_window(window const& wnd)
{
	auto result = unique_ptr<window>::allocate(wnd);
	for (auto const& h : result->get_hints())
		window_hint(h);

	result->m_handle = glfwCreateWindow(result->get_resolution().x, result->get_resolution().y, result->get_title().c_str(), nullptr, nullptr);

	if (result->get_handle() == nullptr)
		return nullptr;

	set_current_context(result.get());
	set_window_callbacks(result.get());
	result->m_frame_buffer_size = get_framebuffer_size(result.get());
	result->m_is_open = true;
#ifdef AGL_DEBUG
	g_logger->debug("Window created: {} \\{{}, {}\\}", result->get_title(), result->get_resolution().x, result->get_resolution().y);
#endif
	m_windows.push_back(std::move(result));
	return m_windows.back().get();
}
void windows_event_resource::window_gain_focus(event e)
{
	AGL_CORE_ASSERT(e.get_window() != nullptr, "event is invalid");

	glfwFocusWindow(e.get_window()->get_handle());
	e.get_window()->m_is_focused = true;
}
void windows_event_resource::window_lose_focus(event e)
{
	AGL_CORE_ASSERT(e.get_window() != nullptr, "event is invalid");

	e.get_window()->m_is_focused = false;
}
void windows_event_resource::window_maximize(event e)
{
	AGL_CORE_ASSERT(e.get_window() != nullptr, "event is invalid");

	glfwMaximizeWindow(e.get_window()->get_handle());
	e.get_window()->m_is_maximized = true;
	e.get_window()->m_is_minimized = false;
}
void windows_event_resource::window_minimize(event e)
{
	AGL_CORE_ASSERT(e.get_window() != nullptr, "event is invalid");

	glfwIconifyWindow(e.get_window()->get_handle());
	e.get_window()->m_is_maximized = false;
	e.get_window()->m_is_minimized = true;
}
void windows_event_resource::window_restore(event e)
{
	AGL_CORE_ASSERT(e.get_window() != nullptr, "event is invalid");

	glfwRestoreWindow(e.get_window()->get_handle());
	e.get_window()->m_is_minimized = false;
}
void windows_event_resource::window_should_close(event e)
{
	AGL_CORE_ASSERT(e.get_window() != nullptr, "event is invalid");

	if (e.get_window()->m_close_next_frame)
		return destroy_window(e.get_window());
	e.get_window()->m_close_next_frame = true;
}
void windows_event_resource::window_hint(window::hint hint)
{
	switch (hint.type)
	{
	case window::hint::DEFAULT: glfwDefaultWindowHints(); break;
	case window::hint::INT:	    glfwWindowHint(static_cast<int>(hint.hint), static_cast<int>(hint.value.integer)); break;
	case window::hint::STRING:  glfwWindowHintString(static_cast<int>(hint.hint), hint.value.string); break;
	default: break;
	}
}
void windows_event_resource::set_window_callbacks(window* wnd)
{
	glfwSetCharCallback(wnd->get_handle(), window_char_callback);
	glfwSetCursorEnterCallback(wnd->get_handle(), window_cursor_enter_callback);
#ifdef AGL_DEBUG
	glfwSetErrorCallback(error_callback);
#endif
	glfwSetFramebufferSizeCallback(wnd->get_handle(), window_framebuffer_size_callback);
	glfwSetScrollCallback(wnd->get_handle(), window_scroll_input_callback);
	glfwSetWindowCloseCallback(wnd->get_handle(), window_close_callback);
	glfwSetWindowIconifyCallback(wnd->get_handle(), window_iconify_callback);
	glfwSetWindowFocusCallback(wnd->get_handle(), window_focus_callback);
	glfwSetWindowSizeCallback(wnd->get_handle(), window_size_callback);
	glfwSetWindowUserPointer(wnd->get_handle(), reinterpret_cast<void*>(wnd));
	glfwSetWindowMaximizeCallback(wnd->get_handle(), window_maximize_callback);
	glfwSetWindowContentScaleCallback(wnd->get_handle(), window_scale_callback);
	glfwSetKeyCallback(wnd->get_handle(), window_key_input_callback);
	glfwSetCursorPosCallback(wnd->get_handle(), window_cursor_position_callback);
	glfwSetMouseButtonCallback(wnd->get_handle(), window_button_input_callback);
}
#ifdef AGL_DEBUG
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
void error_callback(int code, const char* description)
{
	AGL_ASSERT(g_logger != nullptr, "invalid logger pointer");

	g_logger->error("GLFW: {}\n{}", error_to_string(code), description);
}
#endif
// callbacks
// ---------

void window_button_input_callback(GLFWwindow* glfw_handle, int button, int action, int mods)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto const b = event::button{ static_cast<std::uint64_t>(mods), static_cast<button_type>(button) };
	switch (action)
	{
	case GLFW_PRESS:   g_api->push_event(wnd, event::button_pressed_event(wnd, b)); break;
	case GLFW_RELEASE: g_api->push_event(wnd, event::button_released_event(wnd, b)); break;
	}
}
void window_char_callback(GLFWwindow* glfw_handle, unsigned int codepoint)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	g_api->push_event(wnd, event::text_entered_event(wnd, codepoint));
}
void window_close_callback(GLFWwindow* glfw_handle)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	g_api->push_event(wnd, event::window_should_close_event(wnd));
}
void window_cursor_enter_callback(GLFWwindow* glfw_handle, int entered)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	switch (entered)
	{
	case 0:  g_api->push_event(wnd, event::mouse_left_event(wnd)); break;
	default: g_api->push_event(wnd, event::mouse_entered_event(wnd)); break;
	}
}
void window_cursor_position_callback(GLFWwindow* glfw_handle, double xpos, double ypos)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	g_api->push_event(wnd, event::mouse_moved_event(wnd, { xpos, ypos }));
}
void window_focus_callback(GLFWwindow* glfw_handle, int focused)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	switch (focused)
	{
	case 0:  g_api->push_event(wnd, event::window_lost_focus_event(wnd)); break;
	default: g_api->push_event(wnd, event::window_gained_focus_event(wnd)); break;
	}
}
void window_framebuffer_size_callback(GLFWwindow* glfw_handle, int width, int height)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	g_api->push_event(wnd, event::framebuffer_resized_event(wnd, { width, height }));
}
void window_iconify_callback(GLFWwindow* glfw_handle, int iconified)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	switch (iconified)
	{
	case 0:  g_api->push_event(wnd, event::window_restored_event(wnd)); break;
	default: g_api->push_event(wnd, event::window_minimized_event(wnd)); break;
	}
}
void window_key_input_callback(GLFWwindow* glfw_handle, int key, int scancode, int action, int mods)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	auto const k = event::key{ static_cast<std::uint64_t>(mods), static_cast<std::uint64_t>(scancode), static_cast<key_type>(key) };
	switch (action)
	{
	case GLFW_PRESS:   g_api->push_event(wnd, event::key_pressed_event(wnd, k)); break;
	case GLFW_RELEASE: g_api->push_event(wnd, event::key_released_event(wnd, k)); break;
	case GLFW_REPEAT:  g_api->push_event(wnd, event::key_repeated_event(wnd, k)); break;
	}
}
void window_maximize_callback(GLFWwindow* glfw_handle, int maximized)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	switch (maximized)
	{
	case 0:  g_api->push_event(wnd, event::window_restored_event(wnd)); break;
	default: g_api->push_event(wnd, event::window_maximized_event(wnd)); break;
	}
}
void window_scale_callback(GLFWwindow* glfw_handle, float xscale, float yscale)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	g_api->push_event(wnd, event::window_rescaled_event(wnd, { xscale, yscale }));
}

void window_scroll_input_callback(GLFWwindow* glfw_handle, double xoffset, double yoffset)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	g_api->push_event(wnd, event::scroll_moved_event(wnd, { xoffset, yoffset }));
}
void window_size_callback(GLFWwindow* glfw_handle, int width, int height)
{
	auto* wnd = reinterpret_cast<window*>(glfwGetWindowUserPointer(glfw_handle));
	g_api->push_event(wnd, event::framebuffer_resized_event(wnd, { width, height }));
}
}