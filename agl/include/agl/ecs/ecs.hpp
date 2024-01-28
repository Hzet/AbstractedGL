#pragma once
#include <any>
#include "agl/core/application.hpp"
#include "agl/ecs/components.hpp"
#include "agl/ecs/entity.hpp"
#include "agl/ecs/system.hpp"
#include "agl/util/memory/unique-ptr.hpp"

namespace agl
{
namespace ecs
{
class organizer
	: public application_resource
{
private:
	using components = mem::dictionary<type_id_t, mem::unique_ptr<component_storage_base>>;
	using entities = mem::deque<impl::entity_data>;
	using systems = mem::dictionary<type_id_t, mem::unique_ptr<system>>;
	using allocator_type = mem::pool::allocator<organizer>;
	using system_allocator = mem::pool::allocator<system>;
	template <typename T>
	using is_system = std::enable_if_t<std::is_base_of_v<system, T>>;

public:
	organizer(mem::pool::allocator<organizer> allocator) noexcept
		: application_resource(type_id<organizer>::get_id())
		, m_components{ components::allocator_type{ allocator } }
		, m_entities{ 1024, entities::allocator_type{ allocator } }
		, m_systems{ systems::allocator_type{ allocator } }
	{
	}
	organizer(organizer&& other) noexcept
		: m_components{ std::move(other.m_components) }
		, m_entities{ std::move(other.m_entities) }
		, m_systems{ std::move(other.m_systems) }
	{
	}
	organizer& operator=(organizer&& other) noexcept
	{
		m_components = std::move(other.m_components);
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
		auto& storage = m_components[type_id<T>::get_id()];

		if (storage == nullptr)
		{
			storage = mem::make_unique<component_storage_base>(m_components.get_allocator(), component_storage<T>{});
			storage->storage = mem::deque<T>{ sizeof(T) * 500, mem::pool::allocator<T>{ m_components.get_allocator() }};
		}
		auto& deq = *reinterpret_cast<component_storage<T>>(container.get());
		deq.push_back(T{ std::forward<TArgs>(args)... });
		ent.m_data->push_component<T>(&deq.back());
	}
	template <typename T>
	void pop_component(entity& ent, T* ptr) noexcept
	{
		AGL_ASSERT(m_components[type_id<T>::get_id()] != nullptr, "Invalid component type");

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
	template <typename T, typename = is_system<T>>
	bool has_system() const noexcept
	{
		return m_systems.find(type_id<T>::get_id()) != m_systems.cend();
	}
	template <typename T, typename = is_system<T>>
	void add_system(application* app, T sys) noexcept
	{
		AGL_ASSERT(!has_system<T>(), "System already present");

		auto allocator = mem::pool::allocator<T>{ m_systems.get_allocator() };
		auto& s = m_systems[type_id<T>::get_id()];
		s = mem::make_unique<system>(allocator, sys);
		s->on_attach(app);
	}
	virtual void on_update(application* app) noexcept override
	{
		for (auto& sys : m_systems)
			sys.second->on_update(app);
	}
	virtual void on_attach(application*) noexcept override {};
	virtual void on_detach(application*) noexcept override {};
	template <typename T>
	void remove_system(application* app) noexcept
	{
		auto found = m_systems.find(type_id<T>::get_id());

		AGL_ASSERT(found != m_systems.cend(), "Invalid system");

		found->second->on_detach(app);
		m_systems.erase(found);
	}

private:
	components m_components;
	entities m_entities;
	systems m_systems;
};
}
}
