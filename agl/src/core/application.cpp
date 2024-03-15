#include "agl/render/opengl/renderer.hpp"
#include "agl/core/threads.hpp"
#include "agl/core/events.hpp"
#include "agl/memory/pool.hpp"
#include "agl/ecs/ecs.hpp"

namespace agl
{
application::~application()
{
	close();
}

application_resource::application_resource(type_id_t id)
	: m_id{ id }
{
}
type_id_t application_resource::type() const
{
	return m_id;
}

void application::close()
{
	{
		auto& log = get_resource<logger>();
		log.info("Closing...");
	}

	while (!m_resources.empty())
	{
		auto found = m_resources.find(m_resources_order[m_resources_order.size() - 1]);
		found->second->on_detach(this);
		
		std::lock_guard<std::mutex> lock{ *m_mutex };
		m_resources.erase(found);
		m_resources_order.pop_back();
	}
}
application::properties const& application::get_properties() const
{
	return m_properties;
}
bool application::good() const
{
	return m_good;
}
void application::init()
{
	m_mutex = make_unique<std::mutex>();
	
	{ // threads
		auto threads = agl::threads{};
		add_resource(threads);
	}

	{ // LOGGER
		auto log = logger{};
		add_resource(log);
		get_resource<logger>().info("Core: Initializing");
		get_resource<logger>().info("Main thread: {}", std::this_thread::get_id());
	}
	{ // MEMORY POOL
		auto pool = mem::pool{};
		pool.create(1024 * 1024 * 10); // 10 mb 
		add_resource(pool);
	}
	{ // GLFW Events
		auto events = agl::events{};
		add_resource(events);
	}
	{ // ECS
		auto& pool = get_resource<mem::pool>();
		add_resource(ecs::organizer{ pool.make_allocator<ecs::organizer>() });
		auto& organizer = get_resource<ecs::organizer>();
		// OpenGL renderer
		auto renderer = mem::make_unique<ecs::system_base>(pool.make_allocator<opengl::renderer>(), opengl::renderer{});
		organizer.add_system(this, std::move(renderer));
	}

	m_good = true;
	get_resource<logger>().info("Core: OK");
}

void application::run()
{
	auto& log = get_resource<logger>();
	log.info("Opening...");
	m_properties.is_open = true;

	int i = 0;
	while (m_properties.is_open)
	{
		for(auto &r : m_resources)
			r.second->on_update(this);
	}
}
}