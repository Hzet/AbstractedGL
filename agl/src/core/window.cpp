#include "agl/core/window.hpp"
#include "agl/core/windows-resource.hpp"
#include "agl/core/event.hpp"

namespace agl
{
window::window()
	: m_close_next_frame{ false }
	, m_handle{ nullptr }
	, m_is_focused{ false }
	, m_is_maximized{ false }
	, m_is_minimized{ false }
	, m_is_open{ false }
{
}
window::window(glm::uvec2 resolution, std::string const& title)
	: window{}
{
	m_resolution = resolution;
	m_title = title;
}
void window::set_title(std::string const& title)
{
	m_title = title;
}
bool window::should_close() const
{
	return m_should_close;
}
glm::uvec2 const& window::get_resolution() const
{
	return m_resolution;
}
void window::set_resolution(glm::uvec2 const& size)
{
	m_resolution = size;
	m_events.push_back(event::window_resized_event(this, size));
}
vector<event> const& window::get_events() const
{
	return m_events;
}
GLFWwindow* window::get_handle()
{
	return m_handle;
}
void window::maximize()
{
	m_events.push_back(event::window_maximized_event(this));
}
void window::minimize()
{
	m_events.push_back(event::window_minimized_event(this));
}
bool window::poll_event(event& e)
{
	e = m_events.front();
	m_events.pop_front();
	return !m_events.empty();
}
void window::focus()
{
	m_events.push_back(event::window_gained_focus_event(this));
}
void window::unfocus()
{
	m_events.push_back(event::window_lost_focus_event(this));
}
void window::hint_default()
{
	m_hints.clear();
	m_hints.push_back(hint{ hint::DEFAULT });
}
void window::hint_int(std::uint64_t hint, std::int64_t value)
{
	m_hints.push_back({ hint, hint::INT, value });
}
void window::hint_string(std::uint64_t hint, const char* value)
{
	auto h = window::hint{ hint, hint::STRING };
	h.value.string = value;
	m_hints.push_back(h);
}
vector<window::hint> const& window::get_hints() const
{
	return m_hints;
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
std::string const& window::get_title() const
{
	return m_title;
}
glm::uvec2 window::get_frame_buffer_size() const
{
	return m_frame_buffer_size;
}
void window::close()
{
	m_events.push_back(event::window_should_close_event(this));
	m_should_close = true;
}
}