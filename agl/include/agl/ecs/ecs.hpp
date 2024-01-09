#pragma once
#include "agl/ecs/storage.hpp"
#include "agl/ecs/entity.hpp"
#include "agl/util/memory/unique-ptr.hpp"

namespace agl
{
namespace ecs
{
class organizer
{
public:
	organizer() noexcept = default;
	organizer(organizer&& other) noexcept = default;
	organizer(organizer const& other) noexcept = delete;
	organizer& operator=(organizer&& other) noexcept = default;
	organizer& operator=(organizer const& other) noexcept = delete;
	~organizer() noexcept = default;

	entity make_entity()
	{
		auto* ptr = m_entities.push();
		return entity{ ptr };
	}
	
	template <typename T, typename... TArgs>
	void push_component(entity& ent, TArgs&&... args) noexcept
	{
		auto* ptr = m_storage[type_id<T>::get_id()]->push(std::forward<TArgs>(args...));
		ent.m_data->push_component<T>(ptr);
	}

	template <typename T>
	void pop_component(entity& ent, T* ptr) noexcept
	{
		m_storage[type_id<T>::get_id()]->pop(ptr);
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

	

private:
	impl::storage<impl::entity_data> m_entities;
	std::unordered_map<type_id_t, mem::unique_ptr<impl::storage_base>> m_storage;
};
}
}