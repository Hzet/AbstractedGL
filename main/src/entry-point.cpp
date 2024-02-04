#include <iostream>
#include "agl/core/application.hpp"

namespace agl
{
	unique_ptr<application> create_application()
	{
		auto result = make_unique<application>();
		result->init();
		return result;
	}
}

int main(int argc, char **argv)
{
	auto app = agl::create_application();

	try
	{
		app->run();
		app->close();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception\n";
	}
}
