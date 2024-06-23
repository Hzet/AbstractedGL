#include "agl/render/opengl/renderer.hpp"
#include "agl/core/events.hpp"
#include "agl/core/layer.hpp"
#include "agl/core/logger.hpp"
#include "agl/memory/pool.hpp"
#include "agl/ecs/ecs.hpp"
#include <filesystem>

namespace agl
{
application::~application()
{
	destroy();
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
	m_properties.is_open = false;
}
void application::destroy()
{
	{
		auto* logger = get_resource<agl::logger>();
		logger->info("Closing...");
	}
	
	close();
	while (!m_resources.empty())
	{
		m_mutex.lock();
		auto found = m_resources.find(m_resources_order[m_resources_order.size() - 1]);
		m_mutex.unlock();

		found->second->on_detach(this);
		
		m_mutex.lock();
		m_resources.erase(found);
		m_resources_order.pop_back();
		m_mutex.unlock();
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
void application::use_opengl()
{
	deinit_opengl();
}
void application::deinit_opengl()
{

}
void application::add_resource(unique_ptr<resource_base> resource)
{
	auto it = m_resources.end();
	{
		std::lock_guard<std::mutex> lock{ m_mutex };
		m_resources_order.push_back(resource->type());
		it = m_resources.emplace({ resource->type(), std::move(resource) });
	}
	it->second->on_attach(this);
}
bool application::has_resource(type_id_t type)
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	auto found = m_resources.find(type);
	return found != m_resources.end();
}
void application::remove_resource(type_id_t type)
{
	m_mutex.lock();
	auto found = m_resources.find(type);
	AGL_ASSERT(found!= m_resources.end(), "Index out of bounds");
	m_mutex.unlock();

	found->second->on_detach(this);

	m_mutex.lock();
	m_resources.erase(found);
	m_mutex.unlock();
}
resource_base* application::get_resource(type_id_t type)
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	return m_resources.at(type).get();
}
void application::init()
{
	{ // LOGGER
		add_resource(unique_ptr<resource_base>::polymorphic<logger>());
		get_resource<logger>()->info("Core: Initializing");
		get_resource<logger>()->info("Main thread: {}", std::this_thread::get_id());
	}
	{ // MEMORY POOL
		add_resource(unique_ptr<resource_base>::polymorphic<mem::pool>());
	}
	{ // GLFW Events
		add_resource(unique_ptr<resource_base>::polymorphic<event_system>());
	}
	{ // ECS
		auto* pool = get_resource<mem::pool>();
		auto organizer = unique_ptr<resource_base>::polymorphic<ecs::organizer>(pool->make_allocator<ecs::organizer>());
		add_resource(std::move(organizer));
	}
	{ // Layers
		add_resource(unique_ptr<resource_base>::polymorphic<agl::layers>());
	}

	m_good = true;
	get_resource<logger>()->info("Core: OK");
}
std::string application::get_current_path() const
{
	return std::filesystem::current_path().string();
}

void application::run()
{
	auto* logger = get_resource<agl::logger>();
	logger->info("Opening...");
	m_properties.is_open = true;

	int i = 0;
	while (m_properties.is_open)
	{
		for (auto& r : m_resources)
		{
			if (!m_properties.is_open)
				break;

			r.second->on_update(this);
		}
	}
}
}