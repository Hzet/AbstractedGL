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
	void              add_shader(shader const& shader) override;
	window*           create_window(window wnd) override;
	void              destroy_window(window* wnd) override;
	void              remove_shader(shader& index) override;
		              
private:              
	void              render(render_object* object);
	void              on_attach(application* app) override;
	void              on_attach_index_buffer(index_buffer& i_buffer);
	void              on_attach_vertex_array(vertex_array& v_array);
	void              on_component_attach(ecs::entity* e, type_id_t type_id, std::uint64_t index) override;
	void              on_component_detach(ecs::entity* e, type_id_t type_id, std::uint64_t index) override;
	void              on_detach(application*) override;
	void              on_detach_render_object(render_object* r_object) const;
	void              on_update(application*) override;
	void              on_update_index_buffer(index_buffer& i_buffer);
	void              on_update_vertex_array(vertex_array& v_array);
	void              process_events(window* wnd);

private:
	windows_resource* m_windows_resource;
};
}
}