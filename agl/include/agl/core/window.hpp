#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "agl/vector.hpp"

namespace agl
{
class window_event_system;
class event;
class window
{
public:
	struct hint;

public:
						window();
						window(glm::uvec2 resolution, std::string const& title = "");
	void				close();
	void                hint_default();
	void				hint_int(std::uint64_t hint, std::int64_t value);
	void				hint_string(std::uint64_t hint, const char* value);
	vector<hint> const&	get_hints() const;
	void				focus();
	glm::uvec2			get_frame_buffer_size() const;
	GLFWwindow*			get_handle();
	glm::uvec2 const&	get_resolution() const;
	std::string const&	get_title() const;
	bool				is_open() const;
	bool				is_minimized() const;
	bool				is_maximized() const;
	bool				is_focused() const;
	void				maximize();
	void				minimize();
	void				set_resolution(glm::uvec2 const& size);
	void				set_title(std::string const& title);
	void				unfocus();

private:
	friend class window_event_system;

private:
	window_event_system* m_event_system;
	glm::uvec2			 m_frame_buffer_size;
	GLFWwindow*			 m_handle;
	vector<hint>	     m_hints;
	bool				 m_is_focused;
	bool				 m_is_maximized;
	bool				 m_is_minimized;
	bool				 m_is_open;
	glm::uvec2			 m_resolution;
	std::string			 m_title;

public:
	struct hint
	{
		enum hint_type
		{
			DEFAULT,
			INT,
			STRING
		};

		union hint_value
		{
			std::int64_t integer;
			const char* string;
		};

		std::uint64_t hint;
		hint_type     type;
		hint_value    value;
	};
};
}