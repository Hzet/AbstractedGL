#include "agl/ecs/system.hpp"
#include "agl/ecs/ecs.hpp"

namespace agl
{
namespace ecs
{
system_base::system_base()
	: m_organizer{ nullptr }
{
}
system_base::system_base(type_id_t id, std::string const& name, ecs::stage stage)
	: m_id{ id }
	, m_name{ name }
	, m_stage{ stage }
	, m_organizer{ nullptr }
{
}
system_base::system_base(system_base&& other)
	: m_id{ other.m_id }
	, m_name{ other.m_name }
	, m_stage{ other.m_stage }
	, m_organizer{ other.m_organizer }
{
}
system_base& system_base::operator=(system_base&& other)
{
	m_id = other.m_id;
	m_name = m_name;
	m_stage = m_stage;
	m_organizer = other.m_organizer;
	return *this;
}
std::string const& system_base::name() const
{
	return m_name;
}
type_id_t system_base::id() const
{
	return m_id;
}
void system_base::name(std::string const& name)
{
	m_name = name;
}
stage system_base::stage() const
{
	return m_stage;
}
void system_base::stage(ecs::stage s)
{
	m_stage = s;
}
bool system_base::read_signal(std::uint64_t id)
{
	auto found = m_signals.find(id);
	found->value = !found->value; // switch value 
	return !found->value; // return previous 
}
void system_base::set_signal(std::uint64_t id, bool value)
{
	auto found = m_signals.find(id);
	found->value = value;
}
void system_base::create_signal(std::uint64_t id, bool start_value)
{
	m_signals.emplace(id, start_value);
}
organizer& system_base::get_organizer()
{
	AGL_ASSERT(m_organizer != nullptr, "invalid organizer pointer");

	return *m_organizer;
}
void system_base::set_organizer(organizer* org)
{
	m_organizer = org;
}
}
}