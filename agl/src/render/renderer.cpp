#include "agl/render/renderer.hpp"
#include "agl/ecs/ecs.hpp"

namespace agl
{
renderer::renderer(ecs::organizer* organizer, ecs::stage stage)
	: ecs::system<renderer>{ organizer, stage }
	, m_shaders{ get_organizer()->get_allocator() }
	, m_windows{ get_organizer()->get_allocator() }
{

}
void renderer::add_shader(shader const& shader)
{
	m_shaders.push_back(shader);
}
void renderer::create_window(window* wnd)
{
	m_windows.push_back(wnd);
}
void renderer::destroy_window(window* wnd)
{
	auto found = find_window(wnd);
	m_windows.erase(found);
}
mem::vector<shader> const& renderer::get_shaders() const
{
	return m_shaders;
}
mem::vector<window*> const& renderer::get_windows() const
{
	return m_windows;
}
void renderer::remove_shader(shader& shader)
{
	auto found = find_shader(shader);
	m_shaders.erase(found);
}
mem::vector<shader>::const_iterator renderer::find_shader(shader const& shader) const
{
	return std::find_if(m_shaders.cbegin(), m_shaders.cend(), [&shader](auto const& sh) { return sh.get_filepath() == shader.get_filepath(); });
}
mem::vector<window*>::const_iterator renderer::find_window(window* wnd) const
{
	return std::find_if(m_windows.cbegin(), m_windows.cend(), [wnd](auto const* const w) { return w == wnd; });
}
void renderer::on_detach(application* app)
{
	for (auto* wnd : m_windows)
		destroy_window(wnd);

	for (auto& sh : m_shaders)
		remove_shader(sh);
}
}