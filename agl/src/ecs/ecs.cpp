#include "agl/ecs/ecs.hpp"
#include "agl/core/logger.hpp"

namespace agl
{
namespace ecs
{
organizer::organizer(mem::pool::allocator<organizer> allocator)
	: resource<organizer>{}
	, m_allocator{ allocator }
	, m_components{ allocator }
	, m_entities{ allocator }
	, m_systems{ allocator }
{
}
organizer::organizer(organizer&& other)
	: resource<organizer>{ std::move(other) }
	, m_allocator{ std::move(other.m_allocator) }
	, m_components{ std::move(other.m_components) }
	, m_entities{ std::move(other.m_entities) }
	, m_systems{ std::move(other.m_systems) }
{
}
organizer& organizer::operator=(organizer&& other)
{
	this->resource<organizer>::operator=(std::move(other));

	m_allocator = std::move(other.m_allocator);
	m_components = std::move(other.m_components);
	m_entities = std::move(other.m_entities);
	m_systems = std::move(other.m_systems);

	return *this;
}
system_base* organizer::get_system_impl(type_id_t id)
{
	for (auto& sys : m_systems)
		if (sys->id() == id)
			return sys.get();
	return nullptr;
}
system_base const* organizer::get_system_impl(type_id_t id) const
{
	for (auto const& sys : m_systems)
		if (sys->id() == id)
			return sys.get();
	return nullptr;
}
bool organizer::has_system(type_id_t id) const
{
	return get_system_impl(id) != nullptr;
}
system_base& organizer::get_system(type_id_t id)
{
	AGL_ASSERT(has_system(id), "system not found");

	return *get_system_impl(id);
}
void organizer::add_system(application* app, mem::unique_ptr<system_base> sys)
{
	AGL_ASSERT(!has_system(sys->id()), "System already present");

	m_systems.emplace_back(std::move(sys));
	m_systems.back()->set_organizer(this);
	m_systems.back()->on_attach(app);
}
entity organizer::make_entity()
{
	auto data = impl::entity_data{ get_allocator(), m_entities.size() };
	m_entities.push_back(std::move(data));
	return entity{ &m_entities.back() };
}
void organizer::destroy_entity(entity& ent)
{
	if (ent.empty())
		return;
	
	auto type_ids = ent.get_component_ids();
	for (auto type_id : type_ids)
		pop_components(type_id, ent);

	m_entities.erase(ent.m_data);
	ent = entity{};
}
void organizer::pop_component(type_id_t type_id, entity& ent, std::uint64_t index)
{
	AGL_ASSERT(m_components.find(type_id) != m_components.end(), "invalid component type");
	AGL_ASSERT(ent.has_component(type_id), "queried component type is not attached to this entity");
	AGL_ASSERT(index < ent.size(type_id), "queried component type is not attached to this entity");

	auto& components = m_components.at(type_id);
	auto* ptr = ent.m_data->m_components.at(type_id).at(index);
	ent.m_data->pop_component(type_id, index);
	components->pop_component(ptr);
}
void organizer::pop_components(type_id_t type_id, entity& ent)
{
	AGL_ASSERT(m_components.find(type_id) != m_components.end(), "invalid component type");
	AGL_ASSERT(ent.has_component(type_id), "queried component type is not attached to this entity");

	auto& storage = m_components.at(type_id);
	auto ent_components = ent.m_data->m_components.find(type_id);

	AGL_ASSERT(ent_components != ent.m_data->m_components.end(), "entity has no component of type 'type_id'");

	for (auto* ptr : ent_components->second)
		storage->pop_component(ptr);

	ent_components->second.clear();
	ent.m_data->m_components.erase(ent_components);
}
std::uint64_t organizer::get_component_count(type_id_t type_id) const
{
	auto found = m_components.find(type_id);

	if (found == m_components.end())
		return 0;

	return found->second->size();
}
void organizer::on_attach(application* app) 
{
	auto& log = app->get_resource<agl::logger>();
	log.info("ECS: OK");
}
void organizer::on_detach(application* app) 
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
void organizer::on_update(application* app)
{
	for (auto& sys : m_systems)
		sys->on_update(app);
}
typename organizer::allocator_type organizer::get_allocator() const
{
	return m_allocator;
}
}
}
