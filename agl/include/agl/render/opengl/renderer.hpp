#pragma once
#include "agl/render/opengl/call.hpp"
#include "agl/core/logger.hpp"
#include "agl/render/renderer.hpp"
#include "agl/render/opengl/window.hpp"
#include "agl/render/opengl/feature-type.hpp"
#include <GLFW/glfw3.h>

namespace agl
{
namespace opengl
{
namespace impl
{
// class to receive errors and stuff from glfw callback functions in thread safe manner
// it is a queue of error messages
}
// TODO: add stage
class renderer
	: public agl::renderer
{
public:
	renderer();
	renderer(renderer&& other) noexcept;
	renderer& operator=(renderer&& other) noexcept;
	virtual window* create_window(std::string const& title, glm::uvec2 const& resolution, window::update_fun fun) override;
	virtual void on_attach(application* app) noexcept override;
	virtual void on_update(application*) noexcept override;
	virtual void on_detach(application*) noexcept override;
	void make_window_current(window* wnd) noexcept;

private:
	friend void glfw_error_callback(int, const char*);

private:

private:
	static logger* m_logger;

private:
	window* m_current_window;
	application* m_app;
};
}
}