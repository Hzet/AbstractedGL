#pragma once
#include "agl/render/opengl/call.hpp"
#include "agl/render/renderer.hpp"
#include "agl/ecs/ecs.hpp"

namespace agl
{
namespace opengl
{
// TODO: add stage
class renderer
	: public agl::renderer
{
public:
	renderer();
	renderer(renderer&& other);
	renderer& operator=(renderer&& other);

	virtual agl::shader& attach_shader(std::string const& filepath) override;
	virtual agl::window& create_window(glm::uvec2 const& resolution, std::string const& title) override;
	virtual agl::window& get_window(std::uint64_t index) override;

private:
	virtual void on_attach(application* app) override;
	virtual void on_detach(application*) override;
	virtual void on_update(application*) override;

private:
	ecs::entity m_shaders;
	ecs::entity m_windows;
};
}
}