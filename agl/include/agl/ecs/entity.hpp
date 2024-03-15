#pragma once
#include "agl/util/typeid.hpp"
#include "agl/memory/dictionary.hpp"
#include "agl/memory/vector.hpp"

namespace agl
{
namespace ecs
{
class organizer;

namespace impl
{
class entity_data
{
public:
	entity_data(mem::dictionary<type_id_t, mem::vector<std::byte*>>::allocator_type const& allocator = {}, std::uint64_t index = std::numeric_limits<std::uint64_t>::max()) noexcept;
	entity_data(entity_data&&) = default;
	entity_data& operator=(entity_data&&) = default;

	bool has_component(type_id_t type_id) const noexcept;

	vector<type_id_t> get_component_ids() const noexcept;
	
	template <typename T>
	T& get_component(std::uint64_t index) noexcept;

	template <typename T>
	T const& get_component(std::uint64_t index) const noexcept;

	void pop_components(type_id_t type_id) noexcept;
	void pop_component(type_id_t type_id, std::uint64_t index) noexcept;

	template <typename T>
	void push_component(T* ptr) noexcept;

	std::uint64_t size(type_id_t type_id) const noexcept;

	bool is_valid() const noexcept;

private:
	friend class ecs::organizer;

private:
	template <typename TTuple, std::uint64_t... TSequence>
	void has_component_impl(std::index_sequence<TSequence...>) const noexcept;

private:
	std::uint64_t m_index;
	mem::dictionary<type_id_t, mem::vector<std::byte*>> m_components;

};
}
class entity
{
public:
	entity(impl::entity_data* data = nullptr) noexcept;
	
	template <typename T>
	bool has_component() const noexcept;
	bool has_component(type_id_t type_id) const noexcept;

	vector<type_id_t> get_component_ids() const noexcept;
	template <typename T>
	T& get_component(std::uint64_t index) noexcept;

	template <typename T>
	T const& get_component(std::uint64_t index) const noexcept;

	template <typename T>
	std::uint64_t size() const noexcept;
	std::uint64_t size(type_id_t type_id) const noexcept;

	bool empty() const noexcept;

private:
	friend class organizer;

private:
	impl::entity_data* m_data;
};

namespace impl
{
template <typename T>
T& entity_data::get_component(std::uint64_t index) noexcept
{
	return *reinterpret_cast<T*>(m_components.at(type_id<T>::get_id())[index]);
}

template <typename T>
T const& entity_data::get_component(std::uint64_t index) const noexcept
{
	return *reinterpret_cast<T*>(m_components.at(type_id<T>::get_id())[index]);
}
template <typename T>
void entity_data::push_component(T* ptr) noexcept
{
	auto& components = m_components[type_id<T>::get_id()];

	if (components.empty())
	{
		auto alloc = mem::pool::allocator<std::byte*>{ m_components.get_allocator() };
		components = mem::vector<std::byte*>{ alloc };
	}

	m_components[type_id<T>::get_id()].push_back(reinterpret_cast<std::byte*>(ptr));
}

template <typename TTuple, std::uint64_t... TSequence>
void entity_data::has_component_impl(std::index_sequence<TSequence...>) const noexcept
{
	return (... && (m_components.find(type_id<std::tuple_element_t<TSequence, TTuple>>::get_id()) != m_components.cend()));
}
}

template <typename T>
bool entity::has_component() const noexcept
{
	return has_component(type_id<T>::get_id());
}

template <typename T>
T& entity::get_component(std::uint64_t index) noexcept
{
	return m_data->get_component<T>(index);
}

template <typename T>
T const& entity::get_component(std::uint64_t index) const noexcept
{
	return m_data->get_component<T>(index);
}

template <typename T>
std::uint64_t entity::size() const noexcept
{
	return size(type_id<T>::get_id());
}
}
}