#include "agl/ecs/ecs.hpp"
#include "agl/core/logger.hpp"

namespace agl
{
namespace ecs
{
organizer::organizer(mem::pool::allocator<organizer> allocator) noexcept
	: resource<organizer>{}
	, m_allocator{ allocator }
	, m_components{ components::allocator_type{ allocator } }
	, m_entities{ 1024, entities::allocator_type{ allocator } }
	, m_systems{ systems::allocator_type{ allocator } }
{
}
organizer::organizer(organizer&& other) noexcept
	: resource<organizer>{ std::move(other) }
	, m_components{ std::move(other.m_components) }
	, m_entities{ std::move(other.m_entities) }
	, m_systems{ std::move(other.m_systems) }
{
}
organizer& organizer::operator=(organizer&& other) noexcept
{
	this->resource<organizer>::operator=(std::move(other));

	m_components = std::move(other.m_components);
	m_entities = std::move(other.m_entities);
	m_systems = std::move(other.m_systems);

	return *this;
}
system* organizer::get_system_impl(type_id_t id) noexcept
{
	for (auto const& sys : m_systems)
		if (sys->id() == type_id<T>::get_id())
			return sys.get();
	return nullptr;
}
bool organizer::has_system(type_id_t id); const noexcept
{
	return get_system_impl(id) != nullptr;
}
system& organizer::get_system(type_id_t id) noexcept
{
	AGL_ASSERT(has_system(id), "system not found");

	return get_system_impl(id);
}
system& organizer::add_system(application* app, unique_ptr<system_base> sys) noexcept
{
	AGL_ASSERT(!has_system(sys->id()), "System already present");

	m_systems.emplace_back(std::move(sys));
	m_systems.back()->on_attach(app);
}
entity organizer::make_entity()
{
	auto data = impl::entity_data{ m_entities.get_allocator() };
	m_entities.push_back(std::move(data));
	return entity{ &m_entities.back() };
}
void organizer::on_attach(application* app) noexcept 
{
	auto& log = app->get_resource<agl::logger>();
	log.info("ECS: OK");
}
void organizer::on_detach(application* app) noexcept 
{
	auto& log = app->get_resource<agl::logger>();

	while (!m_systems.empty())
	{
		m_systems.back()->on_detach(app);
		m_systems.erase(m_systems.cend() - 1);
	}
	m_entities.clear();
	m_components.clear();

	log.info("ECS: OFF");
}
void organizer::on_update(application* app) noexcept
{
	for (auto& sys : m_systems)
		sys->on_update(app);
}
typename organizer::allocator_type organizer::get_allocator() const noexcept
{
	return m_allocator;
}
}
}
