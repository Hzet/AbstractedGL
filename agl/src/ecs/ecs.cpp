#include "agl/ecs/ecs.hpp"

namespace agl
{
namespace ecs
{
/*
void organizer::add_system(system sys) noexcept
{
	constexpr auto const comparator = [](system const& s, std::uint64_t stage) { return s.stage < stage; };
	auto const it = std::lower_bound(m_systems.cbegin(), m_systems.cend(), sys.stage, comparator);
	m_systems.insert(it, sys);
}

void organizer::update(organizer* org, float dt) noexcept
{
	for (auto& sys : m_systems)
		sys.fun(org, dt);
}
*/
}
}