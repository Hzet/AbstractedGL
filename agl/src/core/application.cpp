#include "agl/core/application.hpp"
#include "agl/util/logger.hpp"
#include "agl/util/memory/pool.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/ecs/systems/gl-renderer.hpp"

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
		{ // LOGGER
			auto log = logger{};
			add_resource(log);
			get_resource<logger>().info("Initializing...");
		}
		{ // MEMORY POOL
			auto pool = mem::pool{};
			pool.create(1024 * 1024 * 10); // 10 mb 
			add_resource(pool);
		}
		{ // ECS
			auto& pool = get_resource<mem::pool>();
			auto organizer = ecs::organizer{ pool.make_allocator<ecs::organizer>() };
			organizer.add_system<opengl_renderer>(this, opengl_renderer{});
			add_resource(organizer);
		}
	}

	void application::run()
	{
		auto& log = get_resource<logger>();
		log.info("Opening...");

		m_properties.is_open = true;

		while (m_properties.is_open)
		{
			for(auto &r : m_resources)
				r.second->on_update(this);
		}

		log.info("Closing...");
	}
}