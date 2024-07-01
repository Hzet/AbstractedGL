#include "agl/render/opengl/util.hpp"	
#include "agl/render/opengl/renderer.hpp"
#include "agl/core/logger.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/core/windows-resource.hpp"
#include "agl/core/window.hpp"
#include "agl/render/opengl/debug.hpp"
#include "agl/util/util.hpp"
#include <fstream>

namespace agl
{
namespace opengl
{
#ifdef AGL_OPENGL_DEBUG
static agl::logger* g_logger = nullptr;

static void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif

renderer::renderer(ecs::organizer* organizer)
	: agl::renderer{ organizer, ecs::RENDER }
{
}
renderer::renderer(renderer&& other)
	: agl::renderer{ std::move(other) }
{
}
renderer& renderer::operator=(renderer&& other)
{
	this->agl::renderer::operator=(std::move(other));
	return *this;
}
void renderer::init_vertex_array(vertex_array& v_array)
{
	auto id = std::uint32_t{};
	AGL_OPENGL_CALL(glGenBuffers(1, &id));
	v_array.set_buffer_id(id);
	AGL_OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER, v_array.get_bytes_size(), v_array.data(), GL_STATIC_DRAW));

	id = 0;
	AGL_OPENGL_CALL(glGenVertexArrays(1, &id));

	for (auto i = std::uint32_t{}; i < v_array.get_row_layout_size(); ++i)
	{
		auto const ri = v_array.get_row_info(i);
		AGL_OPENGL_CALL(glVertexAttribPointer(i, static_cast<std::uint32_t>(ri.var_count), gl_render_data_type(ri.data_type), ri.normalized, static_cast<std::uint32_t>(v_array.size()), reinterpret_cast<void*>(v_array.get_offset(i))));
		AGL_OPENGL_CALL(glEnableVertexAttribArray(i));
	}

	v_array.set_state(buffer_state::READY);

#ifdef AGL_OPENGL_DEBUG
	g_logger->debug("OpenGL: new vertex array of size {}", agl::util::ns::memory_size(v_array.get_bytes_size()));
#endif
}
void renderer::on_attach(application* app)
{
	auto* logger = app->get_resource<agl::logger>();

#ifdef AGL_OPENGL_DEBUG
	g_logger = app->get_resource<agl::logger>();
#endif
	m_event_system = app->get_resource<agl::windows_resource>();
	logger->info("OpenGL: renderer OK");
}
// render
void renderer::on_update(application* app)
{
	auto const size = m_event_system->get_windows().size();
	for(auto i = 0; i < size; ++i)
	{
		auto* wnd = m_event_system->get_window(i);
		process_events(wnd);
		m_event_system->set_current_context(wnd);
		AGL_OPENGL_CALL(glClearColor(111, 111, 111, 255));
		AGL_OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT));
		glfwSwapBuffers(wnd->get_handle());
	}

	if (get_windows().empty())
		app->close();
}
void renderer::on_update_vertex_array(vertex_array& v_array)
{
	auto const state = v_array.get_state();
	switch (state)
	{
	case buffer_state::INVALID:
	case buffer_state::CHANGED:
		init_vertex_array(v_array);
	}
}
void renderer::on_detach(application* app)
{
	auto* logger = app->get_resource<agl::logger>();
	logger->debug("OpenGL: renderer Exiting");
	
#ifdef AGL_OPENGL_DEBUG
	g_logger = nullptr;
#endif
	m_event_system = nullptr;
	agl::renderer::on_detach(app);
	logger->debug("OpenGL: renderer OFF");
}
void renderer::process_events(window* wnd)
{
	for (auto e : wnd->get_events())
	{
		switch (e.get_type())
		{
		case WINDOW_SHOULD_CLOSE: deinit_window(e.get_window()); break;
		}
	}
}


#ifdef AGL_OPENGL_DEBUG
void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         g_logger->debug("OpenGL: debug output Severity [{}]. Message: {}.", "HIGH", message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:       g_logger->debug("OpenGL: debug output Severity [{}]. Message: {}.", "MEDIUM", message); return;
	case GL_DEBUG_SEVERITY_LOW:          g_logger->debug("OpenGL: debug output Severity [{}]. Message: {}.", "LOW", message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: g_logger->debug("OpenGL: debug output Severity [{}]. Message: {}.", "NOTIFICATION", message); return;
	default: break;
	}
}
#endif

struct shader_source
{
	shader_type type;
	std::string source;
};

static std::uint32_t         compile_subshader(shader_type type, std::string const& source);
static std::uint32_t         link_shader(vector<std::uint32_t> subshaders);
static std::string           load_from_file(std::string const& filepath);
static vector<shader_source> parse_shader_source(std::string const& source);

void renderer::add_shader(shader const& shader)
{
	auto subshaders = vector<std::uint32_t>{};
	auto error_message = std::string{};
	auto error_subshader_type = SHADER_INVALID;
	auto success = std::int32_t{};
	auto file_content = load_from_file(shader.get_filepath());
	auto sources = parse_shader_source(file_content);
	for (auto src : sources)
	{
		auto d = compile_subshader(src.type, src.source);
		AGL_OPENGL_CALL(glGetShaderiv(d, GL_COMPILE_STATUS, &success));
		if (!success)
		{
			error_subshader_type = src.type;
			auto length = std::int32_t{};
			AGL_OPENGL_CALL(glGetShaderiv(d, GL_INFO_LOG_LENGTH, &length));

			error_message.resize(length);
			AGL_OPENGL_CALL(glGetShaderInfoLog(d, length, nullptr, &error_message[0]));
		}
		
		subshaders.push_back(d);
	}
	
	if (!success)
	{
		for (auto& sub : subshaders)
			AGL_OPENGL_CALL(glDeleteShader(sub));
		throw std::exception{ logger::combine_message("Failed to compile subshader {}: {}", get_shader_type_name(error_subshader_type), error_message).c_str() };
	}

	auto descriptor = link_shader(subshaders);
	AGL_OPENGL_CALL(glGetProgramiv(descriptor, GL_LINK_STATUS, &success));
	if (!success)
	{
		auto length = std::int32_t{};
		AGL_OPENGL_CALL(glGetProgramiv(descriptor, GL_INFO_LOG_LENGTH, &length));

		error_message.resize(length);
		AGL_OPENGL_CALL(glGetProgramInfoLog(descriptor, length, NULL, &error_message[0u]));
	}

	for (auto& d : subshaders)
		AGL_OPENGL_CALL(glDeleteShader(d));

	if(!success)
		throw std::exception{ logger::combine_message("Failed to link shader program: \"{}\"", error_message).c_str() };

	agl::renderer::add_shader(agl::shader{shader.get_filepath(), descriptor});

#ifdef AGL_OPENGL_DEBUG
	g_logger->debug("OpenGL: new shader \"{}\"", shader.get_filepath());
#endif
}

void renderer::init_window(window* wnd)
{
	wnd->hint_int(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	wnd->hint_int(GLFW_CONTEXT_VERSION_MAJOR, 4);
	wnd->hint_int(GLFW_CONTEXT_VERSION_MINOR, 3);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::exception{ "Failed to initialize OpenGL context!" };

#ifdef AGL_OPENGL_DEBUG
	AGL_OPENGL_CALL(glEnable(GL_DEBUG_OUTPUT));
	AGL_OPENGL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));

	AGL_OPENGL_CALL(glDebugMessageCallback(gl_debug_callback, nullptr));
	AGL_OPENGL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE));
	g_logger->debug("OpenGL debug messages: ON");
#endif

	agl::renderer::init_window(wnd);
}
void renderer::deinit_window(window* wnd)
{
	agl::renderer::deinit_window(wnd);
}
void renderer::remove_shader(shader& shader)
{
	auto found = find_shader(shader);
	if (found == get_shaders().cend() || found->get_id() == 0)
		return;
		
	shader = *found;
	auto id = static_cast<std::uint32_t>(found->get_id());
	AGL_OPENGL_CALL(glDeleteProgram(id));

	shader = agl::shader{ shader.get_filepath() };
	agl::renderer::remove_shader(shader);
}
std::uint32_t compile_subshader(shader_type type, std::string const& source)
{
	auto descriptor = std::uint32_t{};
	auto const* src = source.c_str();
	AGL_OPENGL_CALL(descriptor = glCreateShader(gl_shader_data_type(type)));
	AGL_OPENGL_CALL(glShaderSource(descriptor, 1, &src, NULL));
	AGL_OPENGL_CALL(glCompileShader(descriptor));

	return descriptor;
}
std::uint32_t link_shader(vector<std::uint32_t> subshaders)
{
	auto descriptor = std::uint32_t{};
	AGL_OPENGL_CALL(descriptor = glCreateProgram());

	for (auto& d : subshaders)
		AGL_OPENGL_CALL(glAttachShader(descriptor, d));

	AGL_OPENGL_CALL(glLinkProgram(descriptor));
	return descriptor;
}
std::string load_from_file(std::string const& filepath)
{
	auto file = std::ifstream{ filepath, std::ios::in };
	auto ss = std::stringstream{};
	file.seekg(0, std::ios::beg);
	ss << file.rdbuf();
	file.close();
	return ss.str();
}
vector<shader_source> parse_shader_source(std::string const& source)
{
	struct keyword
	{
		std::uint64_t index;
		shader_type   type;
	};

	auto keywords = std::vector<keyword>{
		{ std::string::npos, SHADER_COMPUTE },
		{ std::string::npos, SHADER_FRAGMENT },
		{ std::string::npos, SHADER_GEOMETRY },
		{ std::string::npos, SHADER_TESS_CONTROL },
		{ std::string::npos, SHADER_TESS_EVALUATION },
		{ std::string::npos, SHADER_VERTEX },
	};

	for (auto& k : keywords)
		k.index = source.find(get_shader_type_name(k.type));

	std::sort(keywords.begin(), keywords.end(), [](auto const& lhs, auto const& rhs) { return lhs.index < rhs.index; });

	auto result = vector<shader_source>{};
	for (auto i = 0; i < keywords.size(); ++i)
	{
		if (keywords[i].index == std::string::npos)
			break;

		auto src = std::string{};
		if (i == keywords.size() - 1)
			src = source.substr(keywords[i].index);
		else
			src = source.substr(keywords[i].index, keywords[i + 1].index - keywords[i].index);
		src.erase(0, std::strlen(get_shader_type_name(keywords[i].type)) + 1);
		result.push_back({ keywords[i].type, src });
	}

	return result;
}
}
}