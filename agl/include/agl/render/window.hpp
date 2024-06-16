#pragma once
#include "agl/core/window.hpp"
#include <functional>

namespace agl
{
class application;

class window
	: public glfw::window
{
public:
	using glfw::window::window;
};
}