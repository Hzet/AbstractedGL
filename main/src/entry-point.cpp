#include <iostream>
#include "agl/core/application.hpp"

#define AGL_APP_EXEC(stage, instruction) \
	try \
	{ \
		instruction; \
	} \
	catch (std::exception& e) \
	{ \
		std::cerr << "Application error on " stage ": " << e.what() << std::endl; \
		return EXIT_FAILURE; \
	} \
	catch (...) \
	{ \
		std::cerr << "Application error on " stage ":" << "Unknown exception\n"; \
		return EXIT_FAILURE; \
	}

namespace agl
{
int process_application(unique_ptr<application>& app)
{
	AGL_APP_EXEC("initialization", app = create_application());

	if (app == nullptr)
	{
		std::cerr << "Unknown application error\n";
		return EXIT_FAILURE;
	}

	if (!app->good())
	{
		std::cerr << "Unknown application error\n";
		return EXIT_FAILURE;
	}

	AGL_APP_EXEC("execution", app->run());
	return EXIT_SUCCESS;
}
}

int main(int argc, char **argv)
{
	auto app = agl::unique_ptr<agl::application>();
	return agl::process_application(app);
}
