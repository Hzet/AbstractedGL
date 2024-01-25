#pragma once
#include <any>
#include "agl/core/application.hpp"
#include "agl/ecs/entity.hpp"
#include "agl/ecs/system.hpp"
#include "agl/util/deque.hpp"
#include "agl/util/memory/unique-ptr.hpp"

namespace agl
{
namespace ecs
{
class organizer
{
private:
	using allocator_type = mem::pool::allocator<organizer>;
	using component_allocator = typename allocator_type::template rebind<std::pair<type_id_t, std::any>>;
	using system_allocator = typename allocator_type::template rebind<system>;
	using system_vector_allocator = typename allocator_type::template rebind<mem::unique_ptr<system>>;
	using entity_allocator = typename allocator_type::template rebind<impl::entity_data>;

public:
	organizer(mem::pool::allocator<organizer> allocator) noexcept
		: m_components{ component_allocator{ allocator } }
		, m_entities{ 1024, entity_allocator{ allocator } }
		, m_systems{ system_vector_allocator{ allocator } }
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
		{
			auto deq = mem::deque<T>{ sizeof(T) * 500, mem::pool::allocator<T>{ m_components.get_allocator() }};
			container = deq;
		}
		auto& deq = std::any_cast<mem::deque<T>>(container);
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
	mem::vector<entity> view() noexcept
	{
		auto result = mem::vector<entity>{};
		for (auto& e : m_entities)
			if (e.has_component<TArgs...>())
				result.push_back(entity{ &e });

		return result;
	}
	template <typename T, typename TEnable=std::enable_if_t<std::is_base_of_v<system, T>>>
	void add_system(application* app, T&& sys) noexcept
	{
		auto const comp = [](system const& sys, std::uint64_t stage) { return sys.stage < stage; };
		auto it = std::lower_bound(m_systems.begin(), m_systems.end(), sys.stage(), comp);
		it = m_systems.insert(it, std::move(mem::make_unique<system>(system_allocator{ m_systems.get_allocator() }, T{})));
		(*it)->on_attach(app);
	}
	void update(application* app) noexcept
	{
		for (auto& sys : m_systems)
			sys->on_update(app);
	}
	template <typename T>
	void remove_system(application* app) noexcept
	{
		auto const dummy = T{};
		for(auto it = 0; it != m_systems.end(); ++it)
			if (it->name() == dummy.name())
			{
				(*it)->on_detach(app);
				m_systems.erase(it);
 			}
	}

private:
	mem::dictionary<type_id_t, std::any> m_components;
	mem::deque<impl::entity_data> m_entities;
	mem::vector<mem::unique_ptr<system>> m_systems;
};
}
}
