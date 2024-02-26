#include "agl/ecs/system.hpp"

namespace agl
{
namespace ecs
{
system_base::system_base() noexcept
{
}
system_base::system_base(type_id_t id, std::string const& name, ecs::stage stage) noexcept
	: m_id{ id }
	, m_name{ name }
	, m_stage{ stage }
{
}
system_base::system_base(system_base&& other) noexcept
	: m_id{ other.m_id }
	, m_name{ other.m_name }
	, m_stage{ other.m_stage }
{
}
system_base& system_base::operator=(system_base&& other) noexcept
{
	m_id = other.m_id;
	m_name = m_name;
	m_stage = m_stage;
	return *this;
}
std::string const& system_base::name() const noexcept
{
	return m_name;
}
type_id_t system_base::id() const noexcept
{
	return m_id;
}
void system_base::name(std::string const& name)
{
	m_name = name;
}
stage system_base::stage() const noexcept
{
	return m_stage;
}
void system_base::stage(ecs::stage s) noexcept
{
	m_stage = s;
}
bool system_base::read_signal(std::uint64_t id) noexcept
{
	auto found = m_signals.find(id);
	found->value = !found->value; // switch value 
	return !found->value; // return previous 
}
void system_base::set_signal(std::uint64_t id, bool value) noexcept
{
	auto found = m_signals.find(id);
	found->value = value;
}
void system_base::create_signal(std::uint64_t id, bool start_value)
{
	m_signals.emplace(id, start_value);
}
}
}