#include "editor/layer.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/render/opengl/renderer.hpp"
#include "agl/util/random.hpp"

namespace agl
{
namespace editor
{
void layer::on_attach(application* app)
{
	auto& ecs = app->get_resource<agl::ecs::organizer>();
	auto& pool = app->get_resource<agl::mem::pool>();

	auto& renderer = ecs.get_system<agl::renderer>();
	auto& window = renderer.create_window(glm::uvec2{ 800, 600 }, "Editor");
	window.set_clear_color(glm::vec4{ 0.2f });
	renderer.attach_shader("E:\\dev\\c++\\AbstractedGL\\resources\\shader\\basic.glsl");
}
void layer::on_detach(application* app)
{
}
void layer::on_update(application* app)
{
	auto color = glm::vec4{};
	color.x = agl::simple_rand(0.f, 1.f);
	color.y = agl::simple_rand(0.f, 1.f);
	color.z = agl::simple_rand(0.f, 1.f);
	color.w = agl::simple_rand(0.f, 1.f);
	window.set_clear_color(color);
}
}
}