#pragma once
#include "agl/ecs/storage.hpp"
#include "agl/ecs/entity.hpp"
#include "agl/ecs/system.hpp"
#include "agl/util/memory/unique-ptr.hpp"

namespace agl
{
namespace ecs
{
class organizer
{
public:
	organizer(mem::pool::allocator<organizer> allocator) noexcept
		: m_components{ allocator }
		, m_entities{ allocator }
		, m_systems{ allocator }
	{
	}

	organizer(organizer&& other) noexcept
		: m_components{ std::move(m_components) }
		, m_entities{ std::move(other.m_entities) }
		, m_systems{ std::move(other.m_systems) }
	{
	}

	organizer& operator=(organizer&& other) noexcept
	{
		m_components = std::move(m_components);
		m_entities = std::move(other.m_entities);
		m_systems = std::move(other.m_systems);

		return *this;
	}

	organizer(organizer const& other) noexcept = delete;
	organizer& operator=(organizer const& other) noexcept = delete;
	~organizer() noexcept = default;

	entity make_entity()
	{
		auto* ptr = m_entities.push(m_entities.get_allocator());
		return entity{ ptr };
	}
	
	template <typename T, typename... TArgs>
	void push_component(entity& ent, TArgs&&... args) noexcept
	{
		auto& storage_ptr = m_components[type_id<T>::get_id()];
		
		if (storage_ptr == nullptr)
			storage_ptr = mem::make_unique<impl::storage_base>(m_components.get_allocator(), impl::storage<T>{ m_components.get_allocator() });

		auto* storage = reinterpret_cast<impl::storage<T>*>(storage_ptr.get());
		auto* ptr = storage->push(std::forward<TArgs>(args)...);
		ent.m_data->push_component<T>(ptr);
	}

	template <typename T>
	void pop_component(entity& ent, T* ptr) noexcept
	{
		m_components[type_id<T>::get_id()]->pop(ptr);
		ent.m_data->pop_component<T>(ptr);
	}

	template <typename... TArgs>
	mem::vector<entity> view() const noexcept
	{
		auto result = mem::vector<entity>{};
		for (auto const& e : m_entities)
			if (e.has_component<TArgs...>())
				result.push_back(entity{ e });

		return result;
	}

	void add_system(std::uint64_t stage, std::string const& name, system::function fun) noexcept
	{
		constexpr auto const comparator = [](system const& s, std::uint64_t stage) { return s.stage < stage; };
		auto const it = std::lower_bound(m_systems.cbegin(), m_systems.cend(), stage, comparator);
		m_systems.insert(it, system{ fun, name, stage });
	}

	void update(float dt) noexcept
	{
		for (auto& sys : m_systems)
			sys.fun(this, dt);
	}

private:
	mem::dictionary<type_id_t, mem::unique_ptr<impl::storage_base>> m_components;
	impl::storage<impl::entity_data> m_entities;
	mem::vector<system> m_systems;
};
}
}