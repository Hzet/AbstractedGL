#include "agl/core/application.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/render/opengl/renderer.hpp"

namespace agl
{
unique_ptr<application> create_application()
{
	auto app= make_unique<application>();
	app->init();

	auto& ecs = app->get_resource<agl::ecs::organizer>();
	auto& pool = app->get_resource<agl::mem::pool>();

	auto ptr = mem::make_unique<ecs::system_base>(pool, std::move(opengl::renderer{}));
	ecs.add_system(app.get(), std::move(ptr));
	auto& renderer = ecs.get_system<agl::renderer>();
	auto* window = renderer.create_window(
		"Editor",
		glm::uvec2{ 800, 600 },
		{}
	);

	return app;
}
}
