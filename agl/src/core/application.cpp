#include "agl/core/application.hpp"

namespace agl
{

	void application::close()
	{
	}

	application::properties const& application::get_properties() const
	{
		return m_properties;
	}

	void application::init()
	{
	}

	void application::run()
	{
		while (m_properties.is_open)
		{
		}
	}
}