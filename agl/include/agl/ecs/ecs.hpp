#pragma once
#include <any>
#include "agl/core/application.hpp"
#include "agl/ecs/components.hpp"
#include "agl/ecs/entity.hpp"
#include "agl/ecs/system.hpp"
#include "agl/memory/unique-ptr.hpp"
#include "agl/util/typeid.hpp"
#include "agl/util/type-traits.hpp"

namespace agl
{
namespace ecs
{

class organizer
	: public resource<organizer>
{
private:
	using allocator_type = mem::pool::allocator<organizer>;
	using system_allocator = mem::pool::allocator<system_base>;

	template <typename T>
	using is_system_t = std::enable_if_t<std::is_base_of_v<system_base, remove_cvref_t<T>>>;

public:
	organizer(mem::pool::allocator<organizer> allocator) noexcept;
	organizer(organizer&& other) noexcept;
	organizer& operator=(organizer&& other) noexcept;
	~organizer() noexcept = default;

	void add_system(application* app, mem::unique_ptr<system_base> sys);
	template <typename T, typename = is_system_t<T>>
	T& get_system() noexcept;
	system_base& get_system(type_id_t id) noexcept;
	template <typename T, typename = is_system_t<T>>
	bool has_system() const noexcept;
	bool has_system(type_id_t id) const noexcept;
	entity make_entity();
	void destroy_entity(entity& ent);

	template <typename T>
	void pop_components(entity& ent) noexcept;
	void pop_components(type_id_t type_id, entity& ent) noexcept;
	template <typename T>
	void pop_component(entity& ent, std::uint64_t index) noexcept;
	void pop_component(type_id_t type_id, entity& ent, std::uint64_t index) noexcept;
	template <typename T, typename... TArgs>
	void push_component(entity& ent, TArgs... args) noexcept;
	template <typename T>
	std::uint64_t get_component_count() const noexcept;
	std::uint64_t get_component_count(type_id_t type_id) const noexcept;

	template <typename... TArgs>
	mem::vector<entity> view() noexcept;

	template <typename T>
	void remove_system(application* app);

	allocator_type get_allocator() const noexcept;

private:
	system_base* get_system_impl(type_id_t id) noexcept;
	system_base const* get_system_impl(type_id_t id) const noexcept;
	template <typename T>
	mem::deque<T>& get_storage() noexcept;
	virtual void on_attach(application*) override;
	virtual void on_detach(application*) override;
	virtual void on_update(application*) noexcept override;

private:
	allocator_type m_allocator;
	mem::dictionary<type_id_t, mem::unique_ptr<component_storage_base>> m_components;
	mem::deque<impl::entity_data> m_entities;
	mem::vector<mem::unique_ptr<system_base>> m_systems;
};
template <typename T, typename>
T& organizer::get_system() noexcept
{
	auto* ptr = get_system_impl(type_id<T>::get_id());
	auto* result = reinterpret_cast<T*>(ptr);

	AGL_ASSERT(result != nullptr, "invalid system cast");

	return *result;
}

template <typename T>
void organizer::pop_components(entity& ent) noexcept
{
	pop_components(type_id<T>::get_id(), ent);
}
template <typename T, typename... TArgs>
void organizer::push_component(entity& ent, TArgs... args) noexcept
{
	AGL_ASSERT(!ent.empty(), "entity is uninitialized");

	auto& storage = get_storage<T>();
	auto it = storage.emplace_back(std::forward<TArgs>(args)...);
	ent.m_data->push_component<T>(&(*it));
}
template <typename T>
void organizer::pop_component(entity& ent, std::uint64_t index) noexcept
{
	AGL_ASSERT(!ent.empty(), "entity is uninitialized");

	pop_component(type_id<T>::get_id(), ent, index);
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
template <typename T>
std::uint64_t organizer::get_component_count() const noexcept
{
	return get_component_count(type_id<T>::get_id());
}
template <typename T, typename>
bool organizer::has_system() const noexcept
{
	return has_system(type_id_t<T>::get_id());
}
template <typename T>
mem::deque<T>& organizer::get_storage() noexcept
{
	auto& ptr = m_components[type_id<T>::get_id()];
	if (ptr == nullptr)
	{
		auto storage = component_storage<T>{};
		storage.storage = mem::deque<T>{ sizeof(T) * 10, get_allocator() };
		auto allocator = mem::pool::allocator<component_storage<T>>{ get_allocator() };
		ptr = mem::make_unique<component_storage_base>(allocator, std::move(storage));
	}
	auto& storage = *dynamic_cast<component_storage<T>*>(ptr.get());
	return storage.storage;
}
template <typename T>
void organizer::remove_system(application* app)
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
