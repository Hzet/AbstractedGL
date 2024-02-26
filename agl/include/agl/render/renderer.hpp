#pragma once
#include "agl/ecs/system.hpp"
#include "agl/render/window.hpp"

namespace agl
{
// intermediate class to reference when trying to retrieve renderer info, such as for example to get generic window handle
class renderer
	: public ecs::system<renderer>
{
public:
	using system<renderer>::system;
	virtual ~renderer() = default;

	virtual window* create_window(std::string const& title, glm::uvec2 const& resolution, window::update_fun fun) = 0;
	virtual window* get_current_window() noexcept = 0;
};
}