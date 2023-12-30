#ifdef AGL_BUILD_MAIN

#include "agl/core/application.hpp"

namespace agl
{
	std::unique_ptr<application> create_application()
	{
		auto result = std::make_unique<application>();
		result->init();
		return result;
	}
}

int main(int argc, char **argv)
{
	auto app = agl::create_application();

	app->run();
}

#endif