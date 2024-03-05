#pragma once
#include <glm/glm.hpp>
#include "agl/core/application.hpp"

namespace agl
{
struct window_properties
{
	glm::vec4 clear_color;
	bool focused;
	glm::uvec2 frame_buffer_size;
	bool maximized;
	bool minimized;
	glm::uvec2 resolution;
	bool running;
	std::string title;
};
class window
{
public:
	using update_fun = void(*)(agl::application*);

public:
	window() noexcept
		: m_update_fun(nullptr)
	{
	}
	window(std::string const& title, glm::uvec2 const& resolution, update_fun fun)
		: m_update_fun(fun)
	{
		m_properties.title = title;
		m_properties.resolution = resolution;
	}
	virtual ~window() noexcept = default;

	void set_clear_color(glm::vec4 const& color) noexcept
	{
		m_properties.clear_color = color;
	}
	void set_clear_color(float r, float g, float b, float a) noexcept
	{
		set_clear_color({ r, g, b, a });
	}
	virtual void close() = 0;

	window_properties const& get_properties() const noexcept
	{
		return m_properties;
	}

	void on_update(agl::application* app) noexcept
	{
		m_update_fun(app);
	}

protected:
	update_fun m_update_fun;
	window_properties m_properties;
};
}