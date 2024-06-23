#pragma once
#include "agl/core/application.hpp"
#include "agl/core/event.hpp"
#include "agl/vector.hpp"
#include "agl/core/window.hpp"

struct GLFWwindow;
namespace agl
{
class window_event_system
{
public:
	bool				   create_window(window* wnd);
	void				   destroy_window(window* wnd);
	std::uint64_t		   get_events_count() const;
	glm::uvec2             get_framebuffer_size(window* wnd);
	vector<window*> const& get_windows() const;
	bool				   poll_event(event& e);
	event				   view_event(std::uint64_t index);

protected:		  
	void				   clear_events();
	void				   process_event(event e);
						   
private:				   
	void				   push_event(event e);
	void				   set_window_callbacks(window* wnd);
	void				   window_hint(window::hint hint);
	void				   window_gain_focus(event e);
	void				   window_lose_focus(event e);
	void				   window_maximize(event e);
	void				   window_minimize(event e);
	void				   window_restore(event e);

private:
	friend class window;
	friend void  window_button_input_callback(GLFWwindow*, int, int, int);
	friend void  window_char_callback(GLFWwindow*, unsigned int);
	friend void  window_close_callback(GLFWwindow*);
	friend void  window_cursor_enter_callback(GLFWwindow*, int);
	friend void  window_cursor_position_callback(GLFWwindow*, double, double);
	friend void  window_focus_callback(GLFWwindow*, int);
	friend void  window_framebuffer_size_callback(GLFWwindow*, int, int);
	friend void  window_iconify_callback(GLFWwindow*, int);
	friend void  window_key_input_callback(GLFWwindow*, int, int, int, int);
	friend void  window_maximize_callback(GLFWwindow*, int);
	friend void  window_scale_callback(GLFWwindow*, float, float);
	friend void  window_scroll_input_callback(GLFWwindow*, double, double);
	friend void  window_size_callback(GLFWwindow*, int, int);

private:
	vector<event>   m_events;
	vector<event>   m_internal_events;
	vector<window*> m_windows;
};

class event_system
	: public resource<event_system>
	, public window_event_system
{
public:
	     event_system();
		 event_system(event_system&& other) noexcept;
	     ~event_system() noexcept;

private:
	void on_attach(application* app) override;
	void on_detach(application* app) override;
	void on_update(application* app) override;
};
}
