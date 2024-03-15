#pragma once
#include "agl/core/application.hpp"
#include "agl/dictionary.hpp"
#include "agl/util/async.hpp"
#include "agl/render/window.hpp"

namespace agl
{
enum button_type;
struct event;
enum event_type;
enum key_type;

// init and manage GLFW events
class events final
	: public resource<events>
{
public:
	static void set_window_callbacks(window* window);

	events();
	events(events&&) = default;
	events& operator=(events&&) = default;
	~events() noexcept = default;
	void push_event(event e) noexcept;
	void set_button_pressed(button_type button, bool status) noexcept;
	void set_key_pressed(key_type key, bool status) noexcept;
	bool is_button_pressed(button_type button) const noexcept;
	bool is_key_pressed(key_type key) const noexcept;
	event poll_event() noexcept;

public:
	static const vector<button_type> button_types;
	static const vector<key_type> key_types;

private:
	virtual void on_attach(application*) override;
	virtual void on_detach(application*) override;
	virtual void on_update(application*) noexcept override;

private:
	unique_ptr<std::mutex> m_mutex;
	dictionary<button_type, bool> m_button_pressed;
	dictionary<key_type, bool> m_key_pressed;
	vector<event> m_event_queue;
};

enum event_type
{
	INVALID_EVENT,
	MOUSE_SCROLL_MOVED,
	MOUSE_MOVED,
	MOUSE_ENTERED,
	MOUSE_LEFT,
	TEXT_ENTERED,
	WINDOW_RESIZED,
	WINDOW_CLOSED,
	WINDOW_MAXIMIZED,
	WINDOW_MINIMIZED,
	WINDOW_RESTORED,
	WINDOW_RESCALED,
	WINDOW_LOST_FOCUS,
	WINDOW_GAINED_FOCUS
};

enum key_type
{
	UNKNOWN_KEY = -1,
	SPACE = 32,
	APOSTROPHE = 39, /* ' */
	COMMA = 44, /* , */
	MINUS = 45, /* - */
	PERIOD = 46, /* . */
	SLASH = 47, /* / */
	KEY_0 = 48,
	KEY_1 = 49,
	KEY_2 = 50,
	KEY_3 = 51,
	KEY_4 = 52,
	KEY_5 = 53,
	KEY_6 = 54,
	KEY_7 = 55,
	KEY_8 = 56,
	KEY_9 = 57,
	SEMICOLON = 59, /* ; */
	EQUAL = 61, /* = */
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	LEFT_BRACKET = 91, /* [ */
	BACKSLASH = 92, /* \ */
	RIGHT_BRACKET = 93, /* ] */
	GRAVE_ACCENT = 96, /* ` */
	WORLD_1 = 161, /* non-US #1 */
	WORLD_2 = 162, /* non-US #2 */
	ESCAPE = 256,
	ENTER = 257,
	TAB = 258,
	BACKSPACE = 259,
	INSERT = 260,
	DELETE = 261,
	RIGHT = 262,
	LEFT = 263,
	DOWN = 264,
	UP = 265,
	PAGE_UP = 266,
	PAGE_DOWN = 267,
	HOME = 268,
	END = 269,
	CAPS_LOCK = 280,
	SCROLL_LOCK = 281,
	NUM_LOCK = 282,
	PRINT_SCREEN = 283,
	PAUSE = 284,
	F1 = 290,
	F2 = 291,
	F3 = 292,
	F4 = 293,
	F5 = 294,
	F6 = 295,
	F7 = 296,
	F8 = 297,
	F9 = 298,
	F10 = 299,
	F11 = 300,
	F12 = 301,
	F13 = 302,
	F14 = 303,
	F15 = 304,
	F16 = 305,
	F17 = 306,
	F18 = 307,
	F19 = 308,
	F20 = 309,
	F21 = 310,
	F22 = 311,
	F23 = 312,
	F24 = 313,
	F25 = 314,
	KP_0 = 320,
	KP_1 = 321,
	KP_2 = 322,
	KP_3 = 323,
	KP_4 = 324,
	KP_5 = 325,
	KP_6 = 326,
	KP_7 = 327,
	KP_8 = 328,
	KP_9 = 329,
	KP_DECIMAL = 330,
	KP_DIVIDE = 331,
	KP_MULTIPLY = 332,
	KP_SUBTRACT = 333,
	KP_ADD = 334,
	KP_ENTER = 335,
	KP_EQUAL = 336,
	LEFT_SHIFT = 340,
	LEFT_CONTROL = 341,
	LEFT_ALT = 342,
	LEFT_SUPER = 343,
	RIGHT_SHIFT = 344,
	RIGHT_CONTROL = 345,
	RIGHT_ALT = 346,
	RIGHT_SUPER = 347,
	MENU = 348
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

struct event
{
	struct position_factor
	{
		float x;
		float y;
	};

	struct size_factor
	{
		std::uint32_t width;
		std::uint32_t height;
	};

	struct scroll_factor
	{
		float x;
		float y;
	};

	struct keyboard
	{
		key_type code;
		std::int32_t scancode;
		std::int32_t modifiers;
	};

	struct mouse
	{
		button_type code;
		std::int32_t bit_modifiers;
	};

	struct scale_factor
	{
		float x;
		float y;
	};

	event_type type = INVALID_EVENT;
	window* wnd;

	union
	{
		position_factor	position;
		size_factor size;
		scroll_factor scroll;
		keyboard key;
		mouse button;
		scale_factor scale;
		std::uint32_t character;
	};
};
}
