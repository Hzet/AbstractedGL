#pragma once 
#include "agl/core/debug.hpp"
#include <cstdint>
#include <glad/glad.h>

#ifdef AGL_DEBUG
namespace agl
{
namespace opengl
{
namespace impl
{
/// <summary>
/// Read OpenGL error until it is GL_NO_ERROR
/// </summary>
void gl_clear_error();

/// <summary>
/// Check whether the value represents an error
/// </summary>
/// <returns>
/// True - value is not an error
/// False - value represents an error
/// </returns>
bool gl_check_error(std::uint64_t value);

/// <summary>
/// Get the value of an error if such has occurred.
/// </summary>
/// <returns>
/// Last error value
/// </returns>
std::uint64_t gl_get_last_error();
}
}
}

/// <summary>
/// Simplified define to put OpenGL call in. 
/// Asserts on call failure.
/// </summary>
#define AGL_OPENGL_CALL(call) \
		do { \
			using namespace ::agl::opengl::impl; \
			gl_clear_error(); \
			call; \
			if (!gl_check_error(gl_get_last_error())) \
			{ \
				auto code = gl_get_last_error(); \
				AGL_ASSERT(false, "Failed to execute OpenGL call!\n"); \
			} \
		} while(false)
#else
#define AGL_OPENGL_CALL(call) call
#endif