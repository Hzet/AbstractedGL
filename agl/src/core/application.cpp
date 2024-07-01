#include "agl/render/opengl/renderer.hpp"
#include "agl/core/windows-resource.hpp"
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
		m_resources.back()->on_detach(this);
		
		m_mutex.lock();
		m_resources.pop_back();
		m_mutex.unlock();
	}
}
bool application::is_good() const
{
	return m_properties.is_good;
}
bool application::is_open() const
{
	return m_properties.is_open;
}
void application::add_resource(unique_ptr<resource_base> resource)
{
	 m_mutex.lock();
     m_resources.emplace_back(std::move(resource));
	 m_mutex.unlock();

     m_resources.back()->on_attach(this);
}
void application::remove_resource(type_id_t type)
{
	m_resources.back()->on_detach(this);

	m_mutex.lock();
	m_resources.pop_back();
	m_mutex.unlock();
}
resource_base* application::get_resource(type_id_t type)
{
	std::lock_guard<std::mutex> lock{ m_mutex };
	for (auto& ptr : m_resources)
		if (ptr->type() == type)
			return ptr.get();
	return nullptr;
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
		add_resource(unique_ptr<resource_base>::polymorphic<windows_resource>());
	}
	{ // ECS
		auto* pool = get_resource<mem::pool>();
		auto organizer = unique_ptr<resource_base>::polymorphic<ecs::organizer>(pool->make_allocator<ecs::organizer>());
		add_resource(std::move(organizer));
	}
	{ // Layers
		add_resource(unique_ptr<resource_base>::polymorphic<agl::layers>());
	}

	m_properties.is_good = true;
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
	while (is_open())
	{
		for (auto& r : m_resources)
		{
			if (!is_open())
				break;

			r->on_update(this);
		}
	}
}
}