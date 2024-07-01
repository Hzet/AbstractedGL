#pragma once
#include "agl/core/layer.hpp"
#include "agl/core/window.hpp"

namespace agl
{
namespace editor
{
class layer
	: public agl::layer<layer>
{
public:
private:
	void on_attach(application* app) override;
	void on_detach(application* app) override;
	void on_update(application* app) override;

private:
	window* m_window;
};
}
}