#include "agl/ecs/entity.hpp"

namespace agl
{
namespace ecs
{
namespace impl
{
entity_data::entity_data(mem::dictionary<type_id_t, mem::vector<std::byte*>>::allocator_type const& allocator) noexcept
	: m_components{ allocator }
{
}
bool entity_data::has_component(type_id_t id) const noexcept
{
	return m_components.find(id) != m_components.cend();
}

vector<type_id_t> entity_data::get_component_ids() const noexcept
{
	auto result = vector<type_id_t>{};
	result.reserve(m_components.size());

	for (auto const& pair : m_components)
		result.push_back(pair.first);

	return result;
}
}
entity::entity(impl::entity_data* data) noexcept
	: m_data{ data }
{
}

bool entity::has_component(type_id_t id) const noexcept
{
	return m_data->has_component(id);
}

vector<type_id_t> entity::get_component_ids() const noexcept
{
	return m_data->get_component_ids();
}
}
}