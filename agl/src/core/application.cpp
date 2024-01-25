#include "agl/core/application.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/util/memory/pool.hpp"

namespace agl
{

	void application::close()
	{
	}

	application::properties const& application::get_properties() const
	{
		return m_properties;
	}

	resources& application::get_resources() noexcept
	{
		return m_resources;
	}

	void application::init()
	{
		auto pool = mem::pool{};
		pool.create(1024 * 1024 * 10); // 10 mb 
		m_resources.add_resource(this, pool);

		auto organizer = ecs::organizer{ pool.make_allocator<ecs::organizer>() };
		m_resources.add_resource(this, organizer);
	}

	void application::run()
	{
		m_properties.is_open = true;

		while (m_properties.is_open)
		{
			m_resources.update(this);
		}
	}
}