#include "agl/core/application.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/render/opengl/renderer.hpp"
#include <iostream>

namespace agl
{
unique_ptr<application> create_application()
{
	auto app= make_unique<application>();
	app->init();

	auto& ecs = app->get_resource<agl::ecs::organizer>();
	auto& pool = app->get_resource<agl::mem::pool>();

	auto& renderer = ecs.get_system<agl::renderer>();
	auto& window = renderer.create_window(glm::uvec2{ 800, 600 }, "Editor");

	return app;
}
}
