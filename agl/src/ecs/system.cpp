#include "agl/ecs/system.hpp"
#include "agl/ecs/ecs.hpp"
#include "agl/ecs/entity.hpp"

namespace agl
{
namespace ecs
{
system_base::system_base()
	: m_organizer{ nullptr }
{
}
system_base::system_base(organizer* organizer, type_id_t type_id, ecs::stage stage)
	: m_type_id{ type_id }
	, m_stage{ stage }
	, m_organizer{ organizer }
{
}
system_base::system_base(system_base&& other)
	: m_type_id{ other.m_type_id }
	, m_stage{ other.m_stage }
	, m_organizer{ other.m_organizer }
{
}
system_base& system_base::operator=(system_base&& other)
{
	m_type_id = other.m_type_id;
	m_stage = m_stage;
	m_organizer = other.m_organizer;
	return *this;
}
type_id_t system_base::get_type_id() const
{
	return m_type_id;
}
void system_base::on_component_attach(entity* e, type_id_t type_id, std::uint64_t index)
{
}
void system_base::on_component_detach(entity* e, type_id_t type_id, std::uint64_t index)
{
}
stage system_base::stage() const
{
	return m_stage;
}
void system_base::stage(ecs::stage s)
{
	m_stage = s;
}

void system_base::register_signal(type_id_t type_id, signal_message_type message)
{
	m_signals.emplace(signal{ message, type_id });
}
organizer* system_base::get_organizer()
{
	return m_organizer;
}
bool system_base::is_signal_registered(type_id_t type_id, signal_message_type message) const
{
	return m_signals.find(signal{ message, type_id }) != m_signals.cend();
}
void system_base::set_organizer(organizer* org)
{
	m_organizer = org;
}
}
}