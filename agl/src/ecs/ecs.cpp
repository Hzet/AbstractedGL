#include "agl/ecs/ecs.hpp"
#include "agl/core/logger.hpp"

namespace agl
{
namespace ecs
{
organizer::organizer(mem::pool::allocator<organizer> allocator) noexcept
	: application_resource(type_id<organizer>::get_id())
	, m_components{ components::allocator_type{ allocator } }
	, m_entities{ 1024, entities::allocator_type{ allocator } }
	, m_systems{ systems::allocator_type{ allocator } }
{
}
organizer::organizer(organizer&& other) noexcept
	: m_components{ std::move(other.m_components) }
	, m_entities{ std::move(other.m_entities) }
	, m_systems{ std::move(other.m_systems) }
{
}
organizer& organizer::operator=(organizer&& other) noexcept
{
	m_components = std::move(other.m_components);
	m_entities = std::move(other.m_entities);
	m_systems = std::move(other.m_systems);

	return *this;
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
	log.info("ECS: OFF");
}
void organizer::on_update(application* app) noexcept
{
	for (auto& sys : m_systems)
		sys.second->on_update(app);
}
}
}
