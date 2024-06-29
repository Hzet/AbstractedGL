#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include "agl/memory/allocator.hpp"
#include "agl/util/timestamp.hpp"

namespace agl
{
enum event_type
{
	APPLICATION_CLOSE,
	BUTTON_PRESSED,
	BUTTON_RELEASED,
	KEY_PRESSED,
	KEY_RELEASED,
	KEY_REPEATED,
	FRAMEBUFFER_RESIZED,
	INVALID_EVENT,
	MOUSE_SCROLL_MOVED,
	MOUSE_MOVED,
	MOUSE_ENTERED,
	MOUSE_LEFT,
	TEXT_ENTERED,
	WINDOW_CREATED,
	WINDOW_CLOSED,
	WINDOW_GAINED_FOCUS,
	WINDOW_LOST_FOCUS,
	WINDOW_MAXIMIZED,
	WINDOW_MINIMIZED,
	WINDOW_RESCALED,
	WINDOW_RESIZED,
	WINDOW_RESTORED,
	WINDOW_SHOULD_CLOSE,
};

enum key_type
{
	UNKNOWN_KEY		= -1,
	SPACE			= 32,
	APOSTROPHE		= 39, /* ' */
	COMMA			= 44, /* , */
	MINUS			= 45, /* - */
	PERIOD			= 46, /* . */
	SLASH			= 47, /* / */
	KEY_0			= 48,
	KEY_1			= 49,
	KEY_2			= 50,
	KEY_3			= 51,
	KEY_4			= 52,
	KEY_5			= 53,
	KEY_6			= 54,
	KEY_7			= 55,
	KEY_8			= 56,
	KEY_9			= 57,
	SEMICOLON		= 59, /* ; */
	EQUAL			= 61, /* = */
	A				= 65,
	B				= 66,
	C				= 67,
	D				= 68,
	E				= 69,
	F				= 70,
	G				= 71,
	H				= 72,
	I				= 73,
	J				= 74,
	K				= 75,
	L				= 76,
	M				= 77,
	N				= 78,
	O				= 79,
	P				= 80,
	Q				= 81,
	R				= 82,
	S				= 83,
	T				= 84,
	U				= 85,
	V				= 86,
	W				= 87,
	X				= 88,
	Y				= 89,
	Z				= 90,
	LEFT_BRACKET	= 91, /* [ */
	BACKSLASH		= 92, /* \ */
	RIGHT_BRACKET	= 93, /* ] */
	GRAVE_ACCENT	= 96, /* ` */
	WORLD_1			= 161, /* non-US #1 */
	WORLD_2			= 162, /* non-US #2 */
	ESCAPE			= 256,
	ENTER			= 257,
	TAB				= 258,
	BACKSPACE		= 259,
	INSERT			= 260,
	DELETE			= 261,
	RIGHT			= 262,
	LEFT			= 263,
	DOWN			= 264,
	UP				= 265,
	PAGE_UP			= 266,
	PAGE_DOWN		= 267,
	HOME			= 268,
	END				= 269,
	CAPS_LOCK		= 280,
	SCROLL_LOCK		= 281,
	NUM_LOCK		= 282,
	PRINT_SCREEN	= 283,
	PAUSE			= 284,
	F1				= 290,
	F2				= 291,
	F3				= 292,
	F4				= 293,
	F5				= 294,
	F6				= 295,
	F7				= 296,
	F8				= 297,
	F9				= 298,
	F10				= 299,
	F11				= 300,
	F12				= 301,
	F13				= 302,
	F14				= 303,
	F15				= 304,
	F16				= 305,
	F17				= 306,
	F18				= 307,
	F19				= 308,
	F20				= 309,
	F21				= 310,
	F22				= 311,
	F23				= 312,
	F24				= 313,
	F25				= 314,
	KP_0			= 320,
	KP_1			= 321,
	KP_2			= 322,
	KP_3			= 323,
	KP_4			= 324,
	KP_5			= 325,
	KP_6			= 326,
	KP_7			= 327,
	KP_8			= 328,
	KP_9			= 329,
	KP_DECIMAL		= 330,
	KP_DIVIDE		= 331,
	KP_MULTIPLY		= 332,
	KP_SUBTRACT		= 333,
	KP_ADD			= 334,
	KP_ENTER		= 335,
	KP_EQUAL		= 336,
	LEFT_SHIFT		= 340,
	LEFT_CONTROL	= 341,
	LEFT_ALT		= 342,
	LEFT_SUPER		= 343,
	RIGHT_SHIFT		= 344,
	RIGHT_CONTROL	= 345,
	RIGHT_ALT		= 346,
	RIGHT_SUPER		= 347,
	MENU			= 348
};

enum button_type
{
	UNKNOWN_BUTTON = -1,
	BUTTON_LEFT = 0,
	BUTTON_RIGHT = 1,
	BUTTON_MID = 2,
	BUTTON_4 = 3,
	BUTTON_5 = 4,
	BUTTON_6 = 5,
	BUTTON_7 = 6,
	BUTTON_8 = 7
};

class window;
class event final
{
public:
	struct button;
	struct key;
	struct window_hint;
	static event  application_close_event();
	static event  button_pressed_event(window* wnd, button button);
	static event  button_released_event(window* wnd, button button);
	static event  framebuffer_resized_event(window* wnd, glm::uvec2 const& size);
	static event  key_pressed_event(window* wnd, key key);
	static event  key_released_event(window* wnd, key key);
	static event  key_repeated_event(window* wnd, key key);
	static event  scroll_moved_event(window* wnd, glm::ivec2 const& offset);
	static event  mouse_moved_event(window* wnd, glm::dvec2 const& position);
	static event  mouse_entered_event(window* wnd);
	static event  mouse_left_event(window* wnd);
	static event  text_entered_event(window* wnd, char ch);
	static event  window_created_event(window* wnd);
	static event  window_closed_event(window* wnd);
	static event  window_gained_focus_event(window* wnd);
	static event  window_lost_focus_event(window* wnd);
	static event  window_maximized_event(window* wnd);
	static event  window_minimized_event(window* wnd);
	static event  window_rescaled_event(window* wnd, glm::dvec2 const& scale);
	static event  window_resized_event(window* wnd, glm::uvec2 const& size);
	static event  window_restored_event(window* wnd);
	static event  window_should_close_event(window* wnd);
				  
				  event();
				  ~event() noexcept = default;
	button        get_button() const;
	key           get_key() const;
	glm::dvec2    get_mouse_position() const;
	glm::uvec2    get_resolution() const;
	glm::dvec2    get_scale_factor() const;
	glm::ivec2    get_scroll_offset() const;
	timestamp     get_timestamp() const;
	event_type    get_type() const;
	window*       get_window();

public:
	struct button
	{
		std::uint64_t mods;
		button_type   type;
	};

	struct key
	{
		std::uint64_t mods;
		std::uint64_t scancode;
		key_type      type;
	};

private:
	union parameter
	{
		button      button;
		char        character;
		key         key;
		glm::uvec2  uvec2;
		glm::uvec2  ivec2;
		glm::dvec2  dvec2;
	};

private:        
			 event(event_type type, window* wnd);
	explicit event(event_type type, window* wnd, button button);
	explicit event(event_type type, window* wnd, char character);
	explicit event(event_type type, window* wnd, key key);
	explicit event(event_type type, window* wnd, glm::dvec2 const& dvec2);
	explicit event(event_type type, window* wnd, glm::ivec2 const& ivec2);
	explicit event(event_type type, window* wnd, glm::uvec2 const& uvec2);

private:
	parameter			  m_param;
	timestamp             m_timestamp;
	event_type			  m_type   = INVALID_EVENT;
	window*				  m_window = nullptr;

};
}