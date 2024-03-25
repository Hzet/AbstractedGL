#pragma once
#include "agl/core/layer.hpp"
#include "agl/render/window.hpp"

namespace agl
{
namespace editor
{
class layer
	: public agl::layer<layer>
{
public:
private:
	virtual void on_attach(application* app) override;
	virtual void on_detach(application* app) override;
	virtual void on_update(application* app) override;

private:
	window* m_window;
};
}
}