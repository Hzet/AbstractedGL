#pragma once
#include <GLFW/glfw3.h>
#include "agl/render/feature-type.hpp"
#include <glm/glm.hpp>
#include <string>

namespace agl
{
namespace glfw
{
void window_close_callback(GLFWwindow*);
void window_focus_callback(GLFWwindow*, int);
void framebuffer_size_callback(GLFWwindow*, int, int);
void window_iconify_callback(GLFWwindow*, int);
void window_maximize_callback(GLFWwindow*, int);
void window_scale_callback(GLFWwindow*, float, float);
void window_size_callback(GLFWwindow*, int, int);
}
class window
{
public:
	window() noexcept;
	window(window&& other) noexcept;
	window& operator=(window&& other) noexcept;
	virtual ~window() noexcept;

	virtual void close() noexcept;
	virtual void create(glm::uvec2 resolution, std::string const& title);
	glm::uvec2 frame_buffer_size() const noexcept;
	GLFWwindow* get_handle() noexcept;
	virtual void hint_api_version(std::uint64_t major, std::uint64_t minor) = 0;
	bool is_minimized() const noexcept;
	bool is_maximized() const noexcept;
	bool is_focused() const noexcept;
	bool is_open() const noexcept;
	bool should_close() const noexcept;
	virtual void resize(glm::uvec2 const& size) noexcept;
	glm::uvec2 const& size() const noexcept;
	std::string const& title() const noexcept;
	virtual std::string const& get_api_version() const noexcept;
	virtual std::string const& get_shading_language_version() const noexcept;
	void set_version(std::string const& api_version, std::string const& shading_language_version) noexcept;

	virtual void feature_disable(feature_type feature) = 0;
	virtual void feature_enable(feature_type feature) = 0;
	virtual bool feature_status(feature_type feature) = 0;

private:
	friend void glfw::framebuffer_size_callback(GLFWwindow*, int, int);
	friend void glfw::window_close_callback(GLFWwindow*);
	friend void glfw::window_focus_callback(GLFWwindow*, int);
	friend void glfw::window_iconify_callback(GLFWwindow*, int);
	friend void glfw::window_maximize_callback(GLFWwindow*, int);
	friend void glfw::window_scale_callback(GLFWwindow*, float, float);
	friend void glfw::window_size_callback(GLFWwindow*, int, int);

private:
	std::string m_api_version;
	glm::vec4 m_clear_color;
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