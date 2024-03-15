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
	using ecs::system<renderer>::system;

	virtual window& create_window(glm::uvec2 const& resolution, std::string const& title) = 0;
	virtual window& get_window(std::uint64_t index = 0) = 0;

};
}