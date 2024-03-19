#pragma once
#include <cstdint>

namespace agl
{
enum clear_type
{
	CLEAR_COLOR = GL_COLOR_BUFFER_BIT,
	CLEAR_DEPTH = GL_DEPTH_BUFFER_BIT,
	CLEAR_STENCIL = GL_STENCIL_BUFFER_BIT,
};
}