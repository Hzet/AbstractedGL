#pragma once
#include "agl/core/application.hpp"
#include "agl/core/event.hpp"
#include "agl/vector.hpp"
#include "agl/core/window.hpp"

struct GLFWwindow;
namespace agl
{
class windows_event_resource
{
public:
	window*				              create_window(window const& wnd);
	void				              destroy_window(window* wnd);
	window*                           get_current_context();
	window const*                     get_current_context() const;
	glm::uvec2                        get_framebuffer_size(window* wnd);
	window*                           get_window(std::uint64_t index);
	vector<unique_ptr<window>> const& get_windows() const;
	void                              hint_default();
	void				              hint_int(std::uint64_t hint, std::int64_t value);
	void				              hint_string(std::uint64_t hint, const char* value);
	void                              set_current_context(window* wnd);
	                                  windows_event_resource();
	virtual                           ~windows_event_resource();
						              
protected:				              
	void				              clear_events();
	void                              destroy_windows();
	void				              process_events();
						              
private:				              
	void				              push_event(window* wnd, event e);
	void				              set_window_callbacks(window* wnd);
	void				              window_gain_focus(event e);
	void				              window_lose_focus(event e);
	void				              window_maximize(event e);
	void				              window_minimize(event e);
	void				              window_restore(event e);
	void                              window_should_close(event e);
						              
private:				              
	friend class                      application;
	friend void                       window_button_input_callback(GLFWwindow*, int, int, int);
	friend void                       window_char_callback(GLFWwindow*, unsigned int);
	friend void                       window_close_callback(GLFWwindow*);
	friend void                       window_cursor_enter_callback(GLFWwindow*, int);
	friend void                       window_cursor_position_callback(GLFWwindow*, double, double);
	friend void                       window_focus_callback(GLFWwindow*, int);
	friend void                       window_framebuffer_size_callback(GLFWwindow*, int, int);
	friend void                       window_iconify_callback(GLFWwindow*, int);
	friend void                       window_key_input_callback(GLFWwindow*, int, int, int, int);
	friend void                       window_maximize_callback(GLFWwindow*, int);
	friend void                       window_scale_callback(GLFWwindow*, float, float);
	friend void                       window_scroll_input_callback(GLFWwindow*, double, double);
	friend void                       window_size_callback(GLFWwindow*, int, int);
							          
private:					          
	window*                           m_current_context;
	vector<unique_ptr<window>>        m_windows;
};

class windows_resource
	: public resource<windows_resource>
	, public windows_event_resource
{
public:
	     windows_resource();
	     ~windows_resource() noexcept;
    bool is_glfw_initialized() const;

private:
	void initialize();
	void deinitialize();
	void on_attach(application* app) override;
	void on_detach(application* app) override;
	void on_update(application* app) override;

private:
	bool m_glfw_initialized;
};
}
