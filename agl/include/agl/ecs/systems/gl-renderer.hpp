#pragma once
#include <GLFW/glfw3.h>
#include "agl/ecs/ecs.hpp"
#include "agl/core/logger.hpp"

namespace agl
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
class opengl_renderer
	: public ecs::system
{
public:
	opengl_renderer()
		: system(type_id<opengl_renderer>::get_id(), "opengl_renderer", {})
	{
	}

	// load opengl and init
	virtual void on_attach(application* app) noexcept override
	{
		auto& log = app->get_resource<logger>();
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
		
	}
private:
};
}