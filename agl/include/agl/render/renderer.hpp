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
	/*
	renderer(renderer&& other) noexcept
		: system<renderer>{ std::move(other) }
	{
	}
	renderer& operator=(renderer&& other) noexcept
	{
		this->ecs::system<renderer>::operator=(std::move(other));
		return *this;
	}

	virtual ~renderer() noexcept = default;
	*/
	virtual window* create_window(std::string const& title, glm::uvec2 const& resolution, window::update_fun fun) = 0;
	//virtual window* get_current_window() noexcept = 0;
};
}