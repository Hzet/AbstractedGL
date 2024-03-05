#include "agl/render/opengl/window.hpp"
#include "agl/core/application.hpp"
#include "agl/core/events.hpp"
#include "agl/core/logger.hpp"
#include <GLFW/glfw3.h>

namespace agl
{
namespace opengl
{
window::window() noexcept
	: agl::window{}
	, m_handle{ nullptr }
{
}
window::window(GLFWwindow* handle, std::string const& title, glm::uvec2 const& resolution, update_fun fun) noexcept
	: agl::window{ title, resolution, fun }
	, m_handle{ handle }
{
	m_properties.running = true;
	m_properties.minimized = false;
	m_properties.maximized = false;
}
window::window(window&& other) noexcept
	: agl::window{ std::move(other) }
	, m_data{ other.m_data }
	, m_clear_type{ other.m_clear_type }
	, m_handle{ other.m_handle }
{
	other.m_handle = nullptr;
}
window& window::operator=(window&& other) noexcept
{
	if (this == &other)
		return *this;

	this->agl::window::operator=(std::move(other));
	m_clear_type = other.m_clear_type;
	m_data = other.m_data;
	m_handle = other.m_handle;
	other.m_handle = nullptr;
	m_update_fun = other.m_on_update;
	return *this;
}
window::~window() noexcept
{
	if (m_handle == nullptr)
		return;

	close();
}

void window::should_close()
{
	AGL_ASSERT(m_handle != nullptr, "invalid window handle");
	AGL_OPENGL_CALL(glfwWindowShouldClose(m_handle));
}
void window::close()
{
	glfwDestroyWindow(m_handle);
	m_properties.running = false;
	m_handle = nullptr;
}
void window::clear()
{
	AGL_OPENGL_CALL(glClearColor(m_properties.clear_color.x, m_properties.clear_color.y, m_properties.clear_color.z, m_properties.clear_color.w));
	AGL_OPENGL_CALL(glClear(m_clear_type));
}
clear_type window::get_clear_type() const
{
	return m_clear_type;
}
void window::set_clear_type(clear_type type)
{
	m_clear_type = type;
}
void window::display()
{
	glfwSwapBuffers(m_handle);
}
void window::disable_feature(feature_type feature)
{
	AGL_OPENGL_CALL(glDisable(feature));
}
void window::enable_feature(feature_type feature)
{
	AGL_OPENGL_CALL(glEnable(feature));
}
GLFWwindow* window::get_handle() noexcept
{
	return m_handle;
}
}
}