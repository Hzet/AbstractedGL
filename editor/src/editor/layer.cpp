#include "agl/render/opengl/call.hpp"
#include "editor/layer.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/core/logger.hpp"
#include "agl/render/opengl/renderer.hpp"
#include "agl/util/random.hpp"

namespace agl
{
namespace editor
{
void layer::on_attach(application* app)
{
	auto& organizer = app->get_resource<ecs::organizer>();
	auto& pool = app->get_resource<agl::mem::pool>();
	
	{ // OpenGL renderer
		auto renderer = mem::make_unique<ecs::system_base>(pool.make_allocator<opengl::renderer>(), opengl::renderer{});
		organizer.add_system(app, std::move(renderer));
	}
	auto& renderer = organizer.get_system<agl::renderer>();
	m_window = &renderer.create_window(glm::uvec2{ 800, 600 }, "Editor");
	m_window->set_clear_color(glm::vec4{ 0.2f });
	renderer.attach_shader("E:\\dev\\c++\\AbstractedGL\\resources\\shader\\basic.glsl");
}
void layer::on_detach(application* app)
{
	m_window = nullptr;
}
void layer::on_update(application* app)
{
	auto mod = glm::mod(-1.4f, 1.f);
	auto const ch = simple_rand(0, 2);
	switch (ch)
	{
	case 0: m_color.x = glm::mod(m_color.x + 0.0021f, 1.f); break;
	case 1: m_color.y = glm::mod(m_color.y + 0.0011f, 1.f); break;
	case 2: m_color.z = glm::mod(m_color.z + 0.0031f, 1.f); break;
	}
	m_window->set_clear_color(m_color);
}
}
}