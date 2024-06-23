#pragma once
#include "agl/render/render-data-type.hpp"

namespace agl
{
enum shader_type;
namespace opengl
{
std::uint32_t gl_render_data_type(render_data_type data_type);
std::uint32_t gl_shader_data_type(shader_type type);
}
}