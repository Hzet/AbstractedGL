#include "agl/core/event.hpp"
#include "agl/core/logger.hpp"
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
const char* event::get_key_name() const
{
	switch (m_param.key.type)
	{
		case SPACE:         return "SPACE";
		case APOSTROPHE:    return "APOSTROPHE";
		case COMMA:         return "COMMA";
		case MINUS:         return "MINUS";
		case PERIOD:        return "PERIOD";
		case SLASH:         return "SLASH";
		case KEY_0:         return "KEY_0";
		case KEY_1:         return "KEY_1";
		case KEY_2:         return "KEY_2";
		case KEY_3:         return "KEY_3";
		case KEY_4:         return "KEY_4";
		case KEY_5:         return "KEY_5";
		case KEY_6:         return "KEY_6";
		case KEY_7:         return "KEY_7";
		case KEY_8:         return "KEY_8";
		case KEY_9:         return "KEY_9";
		case SEMICOLON:     return "SEMICOLON";
		case EQUAL:         return "EQUAL";
		case A:             return "A";
		case B:             return "B";
		case C:             return "C";
		case D:             return "D";
		case E:             return "E";
		case F:             return "F";
		case G:             return "G";
		case H:             return "H";
		case I:             return "I";
		case J:             return "J";
		case K:             return "K";
		case L:             return "L";
		case M:             return "M";
		case N:             return "N";
		case O:             return "O";
		case P:             return "P";
		case Q:             return "Q";
		case R:             return "R";
		case S:             return "S";
		case T:             return "T";
		case U:             return "U";
		case V:             return "V";
		case W:             return "W";
		case X:             return "X";
		case Y:             return "Y";
		case Z:             return "Z";
		case LEFT_BRACKET:  return "LEFT_BRACKET";
		case BACKSLASH:     return "BACKSLASH";
		case RIGHT_BRACKET: return "RIGHT_BRACKET";
		case GRAVE_ACCENT:  return "GRAVE_ACCENT";
		case WORLD_1:       return "WORLD_1";
		case WORLD_2:       return "WORLD_2";
		case ESCAPE:        return "ESCAPE";
		case ENTER:         return "ENTER";
		case TAB:           return "TAB";
		case BACKSPACE:     return "BACKSPACE";
		case INSERT:        return "INSERT";
		case DELETE:        return "DELETE";
		case RIGHT:         return "RIGHT";
		case LEFT:          return "LEFT";
		case DOWN:          return "DOWN";
		case UP:            return "UP";
		case PAGE_UP:       return "PAGE_UP";
		case PAGE_DOWN:     return "PAGE_DOWN";
		case HOME:          return "HOME";
		case END:           return "END";
		case CAPS_LOCK:     return "CAPS_LOCK";
		case SCROLL_LOCK:   return "SCROLL_LOCK";
		case NUM_LOCK:      return "NUM_LOCK";
		case PRINT_SCREEN:  return "PRINT_SCREEN";
		case PAUSE:         return "PAUSE";
		case F1:            return "F1";
		case F2:            return "F2";
		case F3:            return "F3";
		case F4:            return "F4";
		case F5:            return "F5";
		case F6:            return "F6";
		case F7:            return "F7";
		case F8:            return "F8";
		case F9:            return "F9";
		case F10:           return "F10";
		case F11:           return "F11";
		case F12:           return "F12";
		case F13:           return "F13";
		case F14:           return "F14";
		case F15:           return "F15";
		case F16:           return "F16";
		case F17:           return "F17";
		case F18:           return "F18";
		case F19:           return "F19";
		case F20:           return "F20";
		case F21:           return "F21";
		case F22:           return "F22";
		case F23:           return "F23";
		case F24:           return "F24";
		case F25:           return "F25";
		case KP_0:          return "KP_0";
		case KP_1:          return "KP_1";
		case KP_2:          return "KP_2";
		case KP_3:          return "KP_3";
		case KP_4:          return "KP_4";
		case KP_5:          return "KP_5";
		case KP_6:          return "KP_6";
		case KP_7:          return "KP_7";
		case KP_8:          return "KP_8";
		case KP_9:          return "KP_9";
		case KP_DECIMAL:    return "KP_DECIMAL";
		case KP_DIVIDE:     return "KP_DIVIDE";
		case KP_MULTIPLY:   return "KP_MULTIPLY";
		case KP_SUBTRACT:   return "KP_SUBTRACT";
		case KP_ADD:        return "KP_ADD";
		case KP_ENTER:      return "KP_ENTER";
		case KP_EQUAL:      return "KP_EQUAL";
		case LEFT_SHIFT:    return "LEFT_SHIFT";
		case LEFT_CONTROL:  return "LEFT_CONTROL";
		case LEFT_ALT:      return "LEFT_ALT";
		case LEFT_SUPER:    return "LEFT_SUPER";
		case RIGHT_SHIFT:   return "RIGHT_SHIFT";
		case RIGHT_CONTROL: return "RIGHT_CONTROL";
		case RIGHT_ALT:     return "RIGHT_ALT";
		case RIGHT_SUPER:   return "RIGHT_SUPER";
		case MENU:          return "MENU";
		default:            return "UNKNOWN_KEY";
	}
}
event::button event::get_button() const
{
	return m_param.button;
}
const char* event::get_button_name() const
{
	switch (m_param.button.type)
	{
	case BUTTON_LEFT:    return "BUTTON_LEFT ";
	case BUTTON_RIGHT:   return "BUTTON_RIGHT ";
	case BUTTON_MID:     return "BUTTON_MID ";
	case BUTTON_4:       return "BUTTON_4 ";
	case BUTTON_5:       return "BUTTON_5 ";
	case BUTTON_6:       return "BUTTON_6 ";
	case BUTTON_7:       return "BUTTON_7 ";
	case BUTTON_8:       return "BUTTON_8";
	default:             return "UNKNOWN_BUTTON";
	}
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
const char* event::get_name() const
{
	switch (m_type)
	{
	case BUTTON_PRESSED:	  return "BUTTON_PRESSED";
	case BUTTON_RELEASED:	  return "BUTTON_RELEASED";
	case KEY_PRESSED:		  return "KEY_PRESSED";
	case KEY_RELEASED:		  return "KEY_RELEASED";
	case KEY_REPEATED:		  return "KEY_REPEATED";
	case FRAMEBUFFER_RESIZED: return "FRAMEBUFFER_RESIZED";
	case MOUSE_SCROLL_MOVED:  return "MOUSE_SCROLL_MOVED";
	case MOUSE_MOVED:		  return "MOUSE_MOVED";
	case MOUSE_ENTERED:		  return "MOUSE_ENTERED";
	case MOUSE_LEFT:		  return "MOUSE_LEFT";
	case TEXT_ENTERED:		  return "TEXT_ENTERED";
	case WINDOW_CREATED:	  return "WINDOW_CREATED";
	case WINDOW_CLOSED:		  return "WINDOW_CLOSED";
	case WINDOW_GAINED_FOCUS: return "WINDOW_GAINED_FOCUS";
	case WINDOW_LOST_FOCUS:	  return "WINDOW_LOST_FOCUS";
	case WINDOW_MAXIMIZED:	  return "WINDOW_MAXIMIZED";
	case WINDOW_MINIMIZED:	  return "WINDOW_MINIMIZED";
	case WINDOW_RESCALED:	  return "WINDOW_RESCALED";
	case WINDOW_RESIZED:	  return "WINDOW_RESIZED";
	case WINDOW_RESTORED:	  return "WINDOW_RESTORED";
	case WINDOW_SHOULD_CLOSE: return "WINDOW_SHOULD_CLOSE";
	default:                  return "INVALID_EVENT";
	}
}
window* event::get_window() const
{
	return m_window;
}
std::string event::to_string() const
{
	switch (m_type)
	{
	case BUTTON_PRESSED:
	case BUTTON_RELEASED:
		return logger::combine_message("{} {}", get_name(), get_button_name());
	case KEY_PRESSED:
	case KEY_RELEASED:
	case KEY_REPEATED:
		return logger::combine_message("{} {}", get_name(), get_key_name());
	case FRAMEBUFFER_RESIZED:
	case WINDOW_RESIZED:
		return logger::combine_message("{} {}x{}", get_name(), get_resolution().x, get_resolution().y);
	case MOUSE_SCROLL_MOVED:
		return logger::combine_message("{} {}x {}y", get_name(), get_mouse_position().x, get_mouse_position().y);
	case MOUSE_MOVED:
	case WINDOW_RESCALED:
		return logger::combine_message("{} {}x {}y", get_name(), get_scale_factor().x, get_scale_factor().y);
	default:
		return logger::combine_message("{}", get_name());
	}
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