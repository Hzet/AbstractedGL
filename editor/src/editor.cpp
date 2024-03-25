#include "agl/core/application.hpp"
#include "editor/layer.hpp"

namespace agl
{
unique_ptr<application> create_application()
{
	auto app= make_unique<application>();
	app->init();
	auto& layers = app->get_resource<agl::layers>();
	layers.push_layer(make_unique<layer_base>(editor::layer{}));

	return app;
}
}
