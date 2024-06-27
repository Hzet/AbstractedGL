#pragma once
#include "agl/ecs/system.hpp"
#include "agl/memory/vector.hpp"
#include "agl/render/shader.hpp"

namespace agl
{
class window;
// intermediate class to reference when trying to retrieve renderer info, such as for example to get generic window handle
class renderer
	: public ecs::system<renderer>
{
public:
							             renderer(ecs::organizer* organizer = nullptr, ecs::stage stage = {});
	virtual void                         add_shader(shader const& shader);
	virtual void                         create_window(window* wnd);
	virtual void                         destroy_window(window* wnd);
	mem::vector<shader> const&           get_shaders() const;
	mem::vector<window*> const&          get_windows() const;
	virtual void                         remove_shader(shader& shader);

protected:
	mem::vector<shader>::const_iterator  find_shader(shader const& shader) const;
	mem::vector<window*>::const_iterator find_window(window* wnd) const;
	void                                 on_detach(application* app) override;

private:
	mem::vector<shader>  m_shaders;
	mem::vector<window*> m_windows;
};
}