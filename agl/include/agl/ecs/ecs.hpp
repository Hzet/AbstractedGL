/*
#pragma once
#include <any>
#include "agl/ecs/entity.hpp"
#include "agl/ecs/system.hpp"
#include "agl/util/memory/unique-ptr.hpp"
#include "agl/util/deque.hpp"

namespace agl
{
namespace ecs
{
class organizer
{
public:
	organizer(mem::pool::allocator<organizer> allocator) noexcept
		: m_components{ allocator }
		, m_entities{ 1024, allocator }
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
		auto data = impl::entity_data{ m_entities.get_allocator() };
		m_entities.push_back(std::move(data));
		return entity{ &m_entities.back() };
	}
	template <typename T, typename... TArgs>
	void push_component(entity& ent, TArgs&&... args) noexcept
	{
		auto& container = m_components[type_id<T>::get_id()];

		if (!container.has_value())
			container = deque<T, mem::pool::allocator<T>>{ sizeof(T) * 500, m_components.get_allocator() };

		auto& deq = std::any_cast<deque<T, mem::pool::allocator<int>>>(container);
		deq.push_back(T{ std::forward<TArgs>(args)... });
		ent.m_data->push_component<T>(&deq.back());
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
	mem::dictionary<type_id_t, std::any> m_components;
	mem::deque<impl::entity_data> m_entities;
	mem::vector<system> m_systems;
};
}
}
*/