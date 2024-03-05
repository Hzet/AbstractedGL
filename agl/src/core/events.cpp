#include "agl/core/events.hpp"
#include "agl/core/threads.hpp"
#include "agl/core/logger.hpp"

namespace agl
{
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
	std::lock_guard<mutex> lock{ *m_mutex };
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
	std::lock_guard<mutex> lock{ *m_mutex };
	auto e = m_event_queue.front();
	m_event_queue.pop_front();
	return e;
}
void events::on_attach(application* app) noexcept
{
	auto& threads = app->get_resource<agl::threads>();
	m_mutex = &threads.new_mutex();
	
	auto& logger = app->get_resource<agl::logger>();
	logger.info("Events: ON");
}
void events::on_detach(application* app) noexcept
{
	auto& logger = app->get_resource<agl::logger>();
	logger.info("Events: OFF");

	auto& threads = app->get_resource<agl::threads>();
	threads.delete_mutex(*m_mutex);
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

}