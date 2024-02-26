#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include "agl/render/window.hpp"
#include "agl/render/opengl/call.hpp"
#include "agl/render/opengl/clear-type.hpp"
#include "agl/render/opengl/feature-type.hpp"

namespace agl
{
class application;
class events;
class logger;

namespace opengl
{
class window
	: public agl::window
{
public:
	struct data
	{
		agl::events* events;
		agl::logger* logger;
		opengl::window* window;
	};

public:
	window() noexcept;
	window(GLFWwindow* handle, std::string const& title, glm::uvec2 const& resolution, update_fun fun) noexcept;
	window(window&& other) noexcept;
	window& operator=(window&& other) noexcept;
	~window() noexcept;

	void enable_feature(feature_type feature);
	void disable_feature(feature_type feature);
	void should_close();
	void display();
	clear_type get_clear_type() const;
	void set_clear_type(clear_type type);
	void clear();
	virtual void close() override;
	GLFWwindow* get_handle() noexcept;

private:
	friend class renderer;
	friend void glfw_window_close_callback(GLFWwindow*);
	friend void glfw_window_focus_callback(GLFWwindow*, int);
	friend void glfw_framebuffer_size_callback(GLFWwindow*, int, int);
	friend void glfw_window_iconify_callback(GLFWwindow*, int);
	friend void glfw_window_maximize_callback(GLFWwindow*, int);
	friend void glfw_window_scale_callback(GLFWwindow*, float, float);
	friend void glfw_window_size_callback(GLFWwindow*, int, int);

private:
	data m_data;
	clear_type m_clear_type;
	update_fun m_on_update;
	GLFWwindow* m_handle;
};
}
}