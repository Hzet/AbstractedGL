#pragma once
#include "agl/render/opengl/call.hpp"
#include "agl/render/renderer.hpp"
#include "agl/core/event.hpp"
#include "agl/render/buffer.hpp"
#include "agl/ecs/ecs.hpp"

namespace agl
{
enum shader_type;
class windows_resource;
namespace opengl
{
// TODO: add stage
class renderer
	: public agl::renderer
{
public:
	                  renderer(ecs::organizer* organizer = nullptr);
	                  renderer(renderer&& other);
	                  renderer& operator=(renderer&& other);
	void              add_shader(shader const& shader) override;
	window*           create_window(window wnd) override;
	void              destroy_window(window* wnd) override;
	void              remove_shader(shader& index) override;
		              
private:              
	void              init_vertex_array(vertex_array& v_array);
	void              on_update_vertex_array(vertex_array& v_array);
	void              on_attach(application* app) override;
	void              on_detach(application*) override;
	void              on_update(application*) override;
	void              process_events(window* wnd);

private:
	windows_resource* m_windows_resource;
};
}
}