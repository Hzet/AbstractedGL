#include <iostream>
#include "agl/core/application.hpp"

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
