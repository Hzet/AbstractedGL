#include "agl/ecs/system.hpp"

namespace agl
{
namespace ecs
{
system::system() noexcept
{
}
system::system(std::string const& name, ecs::stage stage) noexcept
	: m_name{ name }
	, m_stage{ stage }
{
}
system::system(system&& other) noexcept
	: m_name{ other.m_name }
	, m_stage{ other.m_stage }
{
}
system& system::operator=(system&& other) noexcept
{
	m_name = m_name;
	m_stage = m_stage;
	return *this;
}
std::string const& system::name() const noexcept
{
	return m_name;
}
void system::name(std::string const& name)
{
	m_name = name;
}
stage system::stage() const noexcept
{
	return m_stage;
}
void system::stage(ecs::stage s) noexcept
{
	m_stage = s;
}
}
}