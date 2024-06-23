#include "agl/core/application.hpp"
#include "editor/layer.hpp"

namespace agl
{
unique_ptr<application> create_application()
{
	auto app= unique_ptr<application>::allocate();
	app->init();
	auto* layers = app->get_resource<agl::layers>();
	layers->push_layer(unique_ptr<layer_base>::polymorphic<editor::layer>());
	return app;
}
}
