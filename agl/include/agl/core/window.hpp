#pragma once
#include <GLFW/glfw3.h>
#include "agl/core/event.hpp"
#include "agl/render/clear-type.hpp"
#include "agl/render/feature-type.hpp"
#include "agl/vector.hpp"
#include <glm/glm.hpp>
#include <string>

namespace agl
{
namespace glfw
{
class window
{
public:
	window();
	window(window&& other);
	window& operator=(window&& other);
	virtual ~window();

	virtual void close();
	virtual void create(glm::uvec2 resolution, std::string const& title);
	glm::uvec2 frame_buffer_size() const;
	GLFWwindow* get_handle();
	virtual std::string const& get_api_version() const;
	glm::vec4 const& get_clear_color() const;
	clear_type get_clear_type() const;
	virtual std::string const& get_shading_language_version() const;
	bool is_minimized() const;
	bool is_maximized() const;
	bool is_focused() const;
	bool is_open() const;
	bool poll_event(agl::event& event);
	virtual void resize(glm::uvec2 const& size);
	bool should_close() const;
	void set_clear_type(clear_type type);
	void set_clear_color(glm::vec4 const& color);
	glm::uvec2 const& size() const;
	std::string const& title() const;
	void set_version(std::string const& api_version, std::string const& shading_language_version);

	virtual void feature_disable(feature_type feature) = 0;
	virtual void feature_enable(feature_type feature) = 0;
	virtual bool feature_status(feature_type feature) = 0;

private:
	friend void window_button_input_callback(GLFWwindow*, int, int, int);
	friend void window_char_callback(GLFWwindow*, unsigned int);
	friend void window_close_callback(GLFWwindow*);
	friend void window_cursor_enter_callback(GLFWwindow*, int);
	friend void window_cursor_position_callback(GLFWwindow*, double, double);
	friend void window_focus_callback(GLFWwindow*, int);
	friend void framebuffer_size_callback(GLFWwindow*, int, int);
	friend void window_iconify_callback(GLFWwindow*, int);
	friend void window_key_input_callback(GLFWwindow*, int, int, int, int);
	friend void window_maximize_callback(GLFWwindow*, int);
	friend void window_scale_callback(GLFWwindow*, float, float);
	friend void window_scroll_input_callback(GLFWwindow*, double, double);
	friend void window_size_callback(GLFWwindow*, int, int);

private:
	void push_event(event e);
	void set_callbacks();

private:
	std::string m_api_version;
	glm::vec4 m_clear_color;
	clear_type m_clear_type;
	vector<event> m_event_queue;
	glm::uvec2 m_frame_buffer_size;
	GLFWwindow* m_handle;
	bool m_is_focused;
	bool m_is_maximized;
	bool m_is_minimized;
	bool m_is_open;
	glm::uvec2 m_resolution;
	std::string m_title;
	std::string m_shading_language_version;
	bool m_should_close;
};
}
}