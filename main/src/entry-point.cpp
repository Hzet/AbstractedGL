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

int main(int argc, char **argv)
{
	auto app = agl::unique_ptr<agl::application>();
	AGL_APP_EXEC("initialization", app = agl::create_application());

	if (app == nullptr)
	{
		std::cerr << "Unknown application error\n";
		return EXIT_FAILURE;
	}
	
	if (!app->good())
	{
		std::cerr << "Unknown application error\n";
		AGL_APP_EXEC("exit", app->close());
		return EXIT_FAILURE;
	}

	AGL_APP_EXEC("execution", app->run());
	AGL_APP_EXEC("exit", app->close());
	AGL_APP_EXEC("destroy", app.release());

	return EXIT_SUCCESS;
}
