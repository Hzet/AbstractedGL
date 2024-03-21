#include "agl/core/application.hpp"
#include "editor/layer.hpp"

namespace agl
{
unique_ptr<application> create_application()
{
	auto app= make_unique<application>();
	app->init();



	return app;
}
}
