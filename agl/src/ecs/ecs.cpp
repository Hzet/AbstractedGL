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
void organizer::emit_component_signal(signal_message_type message, entity* e, type_id_t type_id, std::uint64_t index)
{
	for(auto& sys : m_systems)
		if (sys->is_signal_registered(type_id, message))
		{
			switch (message)
			{
			case COMPONENT_ATTACH: sys->on_component_attach(e, type_id, index);	break;
			case COMPONENT_DETACH: sys->on_component_detach(e, type_id, index);	break;
			default: AGL_CORE_ASSERT(false, "invalid argument"); break;
			}
		}
}
system_base* organizer::get_system_impl(type_id_t id)
{
	for (auto& sys : m_systems)
		if (sys->get_type_id() == id)
			return sys.get();
	return nullptr;
}
system_base const* organizer::get_system_impl(type_id_t id) const
{
	for (auto const& sys : m_systems)
		if (sys->get_type_id() == id)
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
	AGL_ASSERT(index < ent.get_count_of(type_id), "queried component type is not attached to this entity");

	auto& components = m_components.at(type_id);
	auto* ptr = ent.m_data->m_components.at(type_id).at(index);
	
	emit_component_signal(COMPONENT_DETACH, &ent, type_id, index);
	ent.m_data->pop_component(type_id, index);
	components->pop_component(ptr);
}
void organizer::pop_components(type_id_t type_id, entity& ent)
{
	for (auto i = 0; i < ent.get_count_of(type_id); ++i)
		pop_component(type_id, ent, i);
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
	auto* logger = app->get_resource<agl::logger>();
	logger->debug("ECS: OK");
}
void organizer::on_detach(application* app) 
{
	auto* logger = app->get_resource<agl::logger>();

	for (auto& e : m_entities)
		destroy_entity(entity{ &e });

	while (!m_systems.empty())
	{
		m_systems.back()->on_detach(app);
		m_systems.erase(m_systems.cend() - 1);
	}
	m_entities.clear();
	m_components.clear();

	logger->debug("ECS: OFF");
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
