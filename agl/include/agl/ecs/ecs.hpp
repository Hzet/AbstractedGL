#pragma once
#include <any>
#include "agl/core/application.hpp"
#include "agl/ecs/components.hpp"
#include "agl/ecs/entity.hpp"
#include "agl/ecs/system.hpp"
#include "agl/memory/unique-ptr.hpp"

namespace agl
{
namespace ecs
{
/**
 * @brief 
 * 
 * @dependencies
 * 'application'
 * 'pool'
 * 'logger'
 */
class organizer
	: public resource<organizer>
{
private:
	using components = mem::dictionary<type_id_t, mem::unique_ptr<component_storage_base>>;
	using entities = mem::deque<impl::entity_data>;
	using systems = mem::vector<mem::unique_ptr<system>>;
	using allocator_type = mem::pool::allocator<organizer>;
	using system_allocator = mem::pool::allocator<system>;
	template <typename T>
	using is_system = std::enable_if_t<std::is_base_of_v<system, T>>;

public:
	organizer(mem::pool::allocator<organizer> allocator) noexcept;
	organizer(organizer&& other) noexcept;
	organizer& operator=(organizer&& other) noexcept;
	~organizer() noexcept = default;

	template <typename T, typename = is_system<T>>
	void add_system(application* app, T sys) noexcept;

	template <typename T, typename = is_system<T>>
	bool has_system() const noexcept;

	entity make_entity();

	template <typename T>
	void pop_component(entity& ent, T* ptr) noexcept;

	template <typename T, typename... TArgs>
	void push_component(entity& ent, TArgs&&... args) noexcept;

	template <typename... TArgs>
	mem::vector<entity> view() noexcept;

	template <typename T>
	void remove_system(application* app) noexcept;

private:
	virtual void on_attach(application*) noexcept override;
	virtual void on_detach(application*) noexcept override;
	virtual void on_update(application*) noexcept override;

private:
	components m_components;
	entities m_entities;
	systems m_systems;
};

template <typename T, typename... TArgs>
void organizer::push_component(entity& ent, TArgs&&... args) noexcept
{
	auto& storage = m_components[type_id<T>::get_id()];

	if (storage == nullptr)
	{
		auto allocator = mem::pool::allocator<T>{ m_components.get_allocator() };
		storage = mem::make_unique<component_storage_base>(allocator, component_storage<T>{});
		storage->storage = mem::deque<T>{ sizeof(T) * 500, std::move(allocator) };
	}
	auto& deq = *reinterpret_cast<component_storage<T>>(container.get());
	auto& component = *deq.emplace_back(std::forward<TArgs>(args)...);
	ent.m_data->push_component<T>(&component);
}
template <typename T>
void organizer::pop_component(entity& ent, T* ptr) noexcept
{
	AGL_ASSERT(m_components[type_id<T>::get_id()] != nullptr, "Invalid component type");

	m_components[type_id<T>::get_id()]->pop(ptr);
	ent.m_data->pop_component<T>(ptr);
}
template <typename... TArgs>
mem::vector<entity> organizer::view() noexcept
{
	auto result = mem::vector<entity>{};
	for (auto& e : m_entities)
		if (e.has_component<TArgs...>())
			result.push_back(entity{ &e });

	return result;
}
template <typename T, typename>
bool organizer::has_system() const noexcept
{
	for (auto const& sys : m_systems)
		if (sys->id() == type_id<T>::get_id())
			return true;
	return false;
}
template <typename T, typename>
void organizer::add_system(application* app, T sys) noexcept
{
	AGL_ASSERT(!has_system<T>(), "System already present");

	auto allocator = mem::pool::allocator<T>{ m_systems.get_allocator() };
	m_systems.push_back(mem::make_unique<system>(allocator, sys));
	m_systems.back()->on_attach(app);
}
template <typename T>
void organizer::remove_system(application* app) noexcept
{
	for (auto it = m_systems.cbegin(); it != m_systems.cend(); ++it)
		if ((*it)->id() == type_id<T>::get_id())
		{
			(*it)->on_detach(app);
			m_systems.erase(it);
			return;
		}

	AGL_ASSERT(false, "system not present");
}
}
}
