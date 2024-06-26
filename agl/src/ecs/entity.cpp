#include "agl/ecs/entity.hpp"

namespace agl
{
namespace ecs
{
namespace impl
{
entity_data::entity_data(mem::dictionary<type_id_t, mem::vector<std::byte*>>::allocator_type const& allocator, std::uint64_t index)
	: m_components{ allocator }
	, m_index{ index }
{
}
bool entity_data::has_component(type_id_t type_id) const
{
	auto components = m_components.find(type_id);
	return components != m_components.cend() && !components->second.empty();
}
void entity_data::pop_components(type_id_t type_id)
{
	auto found = m_components.find(type_id);
	m_components.erase(found);
}
void entity_data::pop_component(type_id_t type_id, std::uint64_t index)
{
	auto& components = m_components.at(type_id);
	components.erase(components.cbegin() + index);
}
vector<type_id_t> entity_data::get_component_ids() const
{
	auto result = vector<type_id_t>{};
	result.reserve(m_components.size());

	for (auto const& pair : m_components)
		result.push_back(pair.first);

	return result;
}
bool entity_data::is_valid() const
{
	return m_index != std::numeric_limits<std::uint64_t>::max();
}
std::uint64_t entity_data::size(type_id_t type_id) const
{
	auto found = m_components.find(type_id);
	if (found == m_components.cend())
		return 0;
	return found->second.size();
}
}
entity::entity(impl::entity_data* data)
	: m_data{ data }
{
}
bool entity::empty() const
{
	return m_data == nullptr;
}
bool entity::has_component(type_id_t id) const
{
	return m_data->has_component(id);
}
vector<type_id_t> entity::get_component_ids() const
{
	return m_data->get_component_ids();
}
std::uint64_t entity::size(type_id_t id) const
{
	return m_data->size(id);
}
}
}