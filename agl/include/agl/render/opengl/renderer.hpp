#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "agl/ecs/ecs.hpp"
#include "agl/core/logger.hpp"
#include "agl/render/renderer.hpp"

namespace agl
{
namespace opengl
{
namespace impl
{
// class to receive errors and stuff from glfw callback functions in thread safe manner
// it is a queue of error messages
class glfw_reporter
{
public:
private:
};
//void error_callback(int error, const char* description);
}
// TODO: add stage
class renderer
	: public agl::renderer
{
public:
	renderer()
		: agl::renderer{ "opengl_renderer", ecs::RENDER }
	{
	}

	virtual window* create_window(std::string const& title, glm::uvec2 const& resolution, window::update_fun fun) override;

	// load opengl and init
	virtual void on_attach(application* app) noexcept override
	{
		m_app = app;
		auto& log = app->get_resource<logger>();
		m_logger = &log;

		log.info("GLFW: Initializing");

		if (!glfwInit())
			app->get_resource<logger>().error("GLFW: Error");
		log.info("GLFW: OK");
	}

	// render
	virtual void on_update(application*) noexcept override {}

	// unload opengl
	virtual void on_detach(application*) noexcept override
	{
		m_logger = nullptr;
	}

private:
	friend void glfw_error_callback(int, const char*);

private:
	void make_window_current(window* wnd) noexcept;

private:
	static logger* m_logger;

private:
	window* m_current_window;
	application* m_app;
};
}
}