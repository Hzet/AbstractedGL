#pragma once
#include "agl/ecs/ecs.hpp"

namespace agl
{
// TODO: add stage
class opengl_renderer
	: public ecs::system
{
public:
	opengl_renderer()
		: system("opengl_renderer")
	{
	}

	// load opengl and init
	virtual void on_attach(application*) noexcept override
	{

	}

	// render
	virtual void on_update(application*) noexcept override;

	// unload opengl
	virtual void on_detach(application*) noexcept override;
private:
};
}