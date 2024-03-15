#include "agl/ecs/entity.hpp"

namespace agl
{
namespace ecs
{
namespace impl
{
entity_data::entity_data(mem::dictionary<type_id_t, mem::vector<std::byte*>>::allocator_type const& allocator, std::uint64_t index) noexcept
	: m_components{ allocator }
	, m_index{ index }
{
}
bool entity_data::has_component(type_id_t type_id) const noexcept
{
	auto components = m_components.find(type_id);
	return components != m_components.cend() && !components->second.empty();
}
void entity_data::pop_components(type_id_t type_id) noexcept
{
	auto found = m_components.find(type_id);
	m_components.erase(found);
}
void entity_data::pop_component(type_id_t type_id, std::uint64_t index) noexcept
{
	auto& components = m_components.at(type_id);
	components.erase(components.cbegin() + index);
}
vector<type_id_t> entity_data::get_component_ids() const noexcept
{
	auto result = vector<type_id_t>{};
	result.reserve(m_components.size());

	for (auto const& pair : m_components)
		result.push_back(pair.first);

	return result;
}
bool entity_data::is_valid() const noexcept
{
	return m_index == std::numeric_limits<std::uint64_t>::max();
}
std::uint64_t entity_data::size(type_id_t type_id) const noexcept
{
	return m_components.at(type_id).size();
}
}
entity::entity(impl::entity_data* data) noexcept
	: m_data{ data }
{
}
bool entity::empty() const noexcept
{
	return m_data == nullptr;
}
bool entity::has_component(type_id_t id) const noexcept
{
	return m_data->has_component(id);
}
vector<type_id_t> entity::get_component_ids() const noexcept
{
	return m_data->get_component_ids();
}
std::uint64_t entity::size(type_id_t id) const noexcept
{
	return m_data->size(id);
}
}
}