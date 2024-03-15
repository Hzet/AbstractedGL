#include "agl/render/window.hpp"
#include "agl/core/events.hpp"

namespace agl
{
window::window() noexcept
	: m_handle{ nullptr }
	, m_is_focused{ false }
	, m_is_maximized{ false }
	, m_is_minimized{ false }
	, m_is_open{ false }
	, m_should_close{ false }
{
}
window::window(window&& other) noexcept
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
window& window::operator=(window&& other) noexcept
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

window::~window() noexcept
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
	events::set_window_callbacks(this);
}
glm::uvec2 const& window::size() const noexcept
{
	return m_resolution;
}
void window::resize(glm::uvec2 const& size) noexcept
{
	m_resolution = size;
}
GLFWwindow* window::get_handle() noexcept
{
	return m_handle;
}
bool window::is_minimized() const noexcept
{
	return m_is_minimized;
}
bool window::is_maximized() const noexcept
{
	return m_is_maximized;
}
bool window::is_focused() const noexcept
{
	return m_is_focused;
}
bool window::is_open() const noexcept
{
	return m_is_open;
}
bool window::should_close() const noexcept
{
	return m_should_close;
}
std::string const& window::get_api_version() const noexcept
{
	return m_api_version;
}
std::string const& window::get_shading_language_version() const noexcept
{
	return m_shading_language_version;
}
void window::set_version(std::string const& api_version, std::string const& shading_language_version) noexcept
{
	m_api_version = api_version;
	m_shading_language_version = shading_language_version;
}
std::string const& window::title() const noexcept
{
	return m_title;
}
glm::uvec2 window::frame_buffer_size() const noexcept
{
	return m_frame_buffer_size;
}
void window::close() noexcept
{
	if (m_handle == nullptr)
		return;

	glfwDestroyWindow(m_handle);
	m_is_open = false;
}
}