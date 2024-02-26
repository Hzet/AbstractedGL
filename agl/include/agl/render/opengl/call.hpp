#pragma once 
#include <glad/glad.h>
#include "agl/core/debug.hpp"

#ifdef AGL_DEBUG
namespace agl
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


/// <summary>
/// Simplified define to put OpenGL call in. 
/// Asserts on call failure.
/// </summary>
#define AGL_OPENGL_CALL(call) \
		do { \
			::agl::gl_clear_error(); \
			call; \
			AGL_ASSERT(::agl::gl_check_error(::agl::gl_get_last_error()), "Failed to execute OpenGL call!\n", "Error code: [", ::agl::gl_get_last_error(), "]\n"); \
		} while(false)
#else
#define AGL_OPENGL_CALL(call) call
#endif