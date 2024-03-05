#include "agl/core/threads.hpp"
#include "agl/memory/pool.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/render/opengl/renderer.hpp"

namespace agl
{
application::~application() noexcept
{
	close();
}

application_resource::application_resource(type_id_t id) noexcept
	: m_id{ id }
{
}
type_id_t application_resource::type() const noexcept
{
	return m_id;
}

void application::close()
{
	AGL_ASSERT(m_mutex != nullptr, "invalid mutex");

	while (!m_resources.empty())
	{
		auto found = m_resources.find(m_resources_order[m_resources_order.size() - 1]);

			//std::lock_guard<std::mutex> lock{ *m_mutex };
		found->second->on_detach(this);
		m_resources.erase(found);
		m_resources_order.pop_back();
	}
}

application::properties const& application::get_properties() const
{
	return m_properties;
}

void application::init()
{
	{ // threads
		auto threads = agl::threads{};
		m_mutex = &threads.new_mutex();
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
	{ // ECS
		auto& pool = get_resource<mem::pool>();
		auto organizer = ecs::organizer{ pool.make_allocator<ecs::organizer>() };
		//organizer.add_system<opengl::renderer>(this, opengl::renderer{});
		add_resource(organizer);
	}

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

	log.info("Closing...");
}
}