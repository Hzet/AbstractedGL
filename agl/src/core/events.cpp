#include "agl/core/events.hpp"
#include "agl/core/threads.hpp"
#include "agl/core/logger.hpp"
#include "agl/render/window.hpp"
#include <GLFW/glfw3.h>

namespace agl
{
namespace glfw
{
static logger* g_logger = nullptr;

void api::on_attach(application* app)
{
	auto& logger = app->get_resource<agl::logger>();
	glfw::g_logger = &logger;

	logger.debug("GLFW: Initializing");

	if (!glfwInit())
	{
		const char* description;
		int code = glfwGetError(&description);
		auto str = std::string{};
		if (description != nullptr)
			str = description;

		logger.error("GLFW: {}", str);
		throw std::exception{ logger::combine_message("Failed to initialize GLFW: {}", str).c_str() };
	}
	logger.debug("GLFW: OK");
}
void api::on_detach(application* app)
{
	glfwTerminate();
	glfw::g_logger = nullptr;

	auto& logger = app->get_resource<agl::logger>();
	logger.debug("GLFW: OFF");
}
void api::on_update(application* app)
{
	glfwPollEvents();
}
constexpr const char* error_to_string(int code)
{
	switch (code)
	{
	case GLFW_NO_CURRENT_CONTEXT: return "GLFW_NO_CURRENT_CONTEXT";
	case GLFW_INVALID_ENUM: return "GLFW_INVALID_ENUM";
	case GLFW_INVALID_VALUE: return "GLFW_INVALID_VALUE";
	case GLFW_OUT_OF_MEMORY: return "GLFW_OUT_OF_MEMORY";
	case GLFW_API_UNAVAILABLE: return "GLFW_API_UNAVAILABLE";
	case GLFW_VERSION_UNAVAILABLE: return "GLFW_VERSION_UNAVAILABLE";
	case GLFW_PLATFORM_ERROR: return "GLFW_PLATFORM_ERROR";
	case GLFW_FORMAT_UNAVAILABLE: return "GLFW_FORMAT_UNAVAILABLE";
	}
	AGL_ASSERT(false, "invalid glfw error code");
	return "GLFW_UNKNOWN_ERROR";
}
void error_callback(int code, const char* description)
{
	AGL_ASSERT(g_logger != nullptr, "invalid logger pointer");

	g_logger->error("GLFW: {}\n{}", error_to_string(code), description);
}
}
}