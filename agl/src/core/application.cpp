#include "agl/render/opengl/renderer.hpp"
#include "agl/core/events.hpp"
#include "agl/core/threads.hpp"
#include "agl/core/layer.hpp"
#include "agl/memory/pool.hpp"
#include "agl/ecs/ecs.hpp"
#include <filesystem>

namespace agl
{
application::~application()
{
	close();
}

resource_base::resource_base(type_id_t id)
	: m_id{ id }
{
}
type_id_t resource_base::type() const
{
	return m_id;
}

void application::close()
{
	{
		auto& log = get_resource<logger>();
		log.info("Closing...");
	}
	
	std::lock_guard<std::mutex> lock{ *m_mutex };

	while (!m_resources.empty())
	{
		auto found = m_resources.find(m_resources_order[m_resources_order.size() - 1]);
		found->second->on_detach(this);
		
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
void application::add_resource(unique_ptr<resource_base> resource)
{
	auto it = m_resources.end();
	{
		std::lock_guard<std::mutex> lock{ *m_mutex };
	
		m_resources_order.push_back(resource->type());
		it = m_resources.emplace({ resource->type(), std::move(resource) });
	}
	it->second->on_attach(this);
}
bool application::has_resource(type_id_t type)
{
	std::lock_guard<std::mutex> lock{ *m_mutex };

	auto found = m_resources.find(type);
	return found != m_resources.end();
}
void application::remove_resource(type_id_t type)
{
	std::lock_guard<std::mutex> lock{ *m_mutex };

	auto found = m_resources.find(type);
	
	AGL_ASSERT(found!= m_resources.end(), "Index out of bounds");

	found->second->on_detach(this);
	m_resources.erase(found);
}
resource_base* application::get_resource(type_id_t type)
{
	std::lock_guard<std::mutex> lock{ *m_mutex };

	return m_resources.at(type).get();
}
void application::init()
{
	m_mutex = make_unique<std::mutex>();
	
	{ // threads
		add_resource(make_unique<resource_base>(threads{}));
	}
	{ // LOGGER
		add_resource(make_unique<resource_base>(logger{}));
		get_resource<logger>().info("Core: Initializing");
		get_resource<logger>().info("Main thread: {}", std::this_thread::get_id());
	}
	{ // MEMORY POOL
		add_resource(make_unique<resource_base>(mem::pool{}));
	}
	{ // GLFW Events
		add_resource(make_unique<resource_base>(glfw::api{}));
	}
	{ // ECS
		auto& pool = get_resource<mem::pool>();
		auto organizer = make_unique<resource_base>(ecs::organizer{ pool.make_allocator<ecs::organizer>() });
		add_resource(std::move(organizer));
	}
	{ // Layers
		add_resource(make_unique<resource_base>(agl::layers{}));
	}

	m_good = true;
	get_resource<logger>().info("Core: OK");
}
std::string application::get_current_path() const
{
	return std::filesystem::current_path().string();
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