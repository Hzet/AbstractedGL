#include "agl/core/event.hpp"
#include "agl/core/window.hpp"
#include "agl/util/clock.hpp"

namespace agl
{
event event::button_pressed_event(window* wnd, button button)
{
	return event{ BUTTON_PRESSED, wnd, button };
}
event event::button_released_event(window* wnd, button button)
{
	return event{ BUTTON_RELEASED, wnd, button };
}
event event::framebuffer_resized_event(window* wnd, glm::uvec2 const& size)
{
	return event{ FRAMEBUFFER_RESIZED, wnd, size };
}
event event::key_pressed_event(window* wnd, key key)
{
	return event{ KEY_PRESSED, wnd, key };
}
event event::key_released_event(window* wnd, key key)
{
	return event{ KEY_RELEASED, wnd, key };
}
event event::key_repeated_event(window* wnd, key key)
{
	return event{ KEY_REPEATED, wnd, key };
}
event event::scroll_moved_event(window* wnd, glm::ivec2 const& offset)
{
	return event{ MOUSE_SCROLL_MOVED, wnd, offset };
}
event event::mouse_moved_event(window* wnd, glm::dvec2 const& position)
{
	return event{ MOUSE_MOVED, wnd, position };
}
event event::mouse_entered_event(window* wnd)
{
	return event{ MOUSE_ENTERED, wnd };
}
event event::mouse_left_event(window* wnd)
{
	return event{ MOUSE_LEFT, wnd };
}
event event::text_entered_event(window* wnd, char ch)
{
	return event{ TEXT_ENTERED, wnd, ch };
}
event event::window_created_event(window* wnd)
{
	return event{ WINDOW_CREATED, wnd };
}
event event::window_closed_event(window* wnd)
{
	return event{ WINDOW_CLOSED, wnd };
}
event event::window_gained_focus_event(window* wnd)
{
	return event{ WINDOW_GAINED_FOCUS, wnd };
}
event event::window_lost_focus_event(window* wnd)
{
	return event{ WINDOW_LOST_FOCUS, wnd };
}
event event::window_maximized_event(window* wnd)
{
	return event{ WINDOW_MAXIMIZED, wnd };
}
event event::window_minimized_event(window* wnd)
{
	return event{ WINDOW_MINIMIZED, wnd };
}
event event::window_rescaled_event(window* wnd, glm::dvec2 const& scale)
{
	return event{ WINDOW_RESCALED, wnd, scale };
}
event event::window_resized_event(window* wnd, glm::uvec2 const& size)
{
	return event{ WINDOW_CLOSED, wnd, size };
}
event event::window_restored_event(window* wnd)
{
	return event{ WINDOW_RESTORED, wnd };
}
event event::window_should_close_event(window* wnd)
{
	return event{ WINDOW_SHOULD_CLOSE, wnd };
}
event_type event::get_type() const
{
	return m_type;
}
event::key event::get_key() const
{
	return m_param.key;
}
event::button event::get_button() const
{
	return m_param.button;
}
glm::ivec2 event::get_scroll_offset() const
{
	return m_param.ivec2;
}
timestamp event::get_timestamp() const
{
	return m_timestamp;
}
glm::dvec2 event::get_mouse_position() const
{
	return m_param.uvec2;
}
window* event::get_window() const
{
	return m_window;
}
glm::dvec2 event::get_scale_factor() const
{
	return m_param.dvec2;
}
glm::uvec2 event::get_resolution() const
{
	return m_param.uvec2;
}
event::event()
	: m_param{ button_type{} }
	, m_timestamp{ clock::get_current_time() }
	, m_type{ INVALID_EVENT }
	, m_window{ nullptr }
{
}
event::event(event_type type, window* wnd)
	: m_param{ button_type{} }
	, m_timestamp{ clock::get_current_time() }
	, m_type{ type }
	, m_window{ wnd }
{
}
event::event(event_type type, window* wnd, button button)
	: event{ type, wnd }
{
	m_param.button = button;
}
event::event(event_type type, window* wnd, char character)
	: event{ type, wnd }
{
	m_param.character = character;
}

event::event(event_type type, window* wnd, key key)
	: event{ type, wnd }
{
	m_param.key = key;
}
event::event(event_type type, window* wnd, glm::ivec2 const& ivec2)
	: event{ type, wnd }
{
	m_param.ivec2 = ivec2;
}
event::event(event_type type, window* wnd, glm::dvec2 const& dvec2)
	: event{ type, wnd }
{
	m_param.dvec2 = dvec2;
}
event::event(event_type type, window* wnd, glm::uvec2 const& uvec2)
	: event{ type, wnd }
{
	m_param.uvec2 = uvec2;
}
}