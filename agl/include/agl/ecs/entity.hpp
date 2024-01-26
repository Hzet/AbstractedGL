#pragma once
#include "agl/util/typeid.hpp"
#include "agl/util/memory/dictionary.hpp"
#include "agl/util/memory/vector.hpp"

namespace agl
{
namespace ecs
{
namespace impl
{
	class entity_data
	{
	public:
		entity_data(mem::dictionary<type_id_t, mem::vector<std::byte*>>::allocator_type const& allocator = {}) noexcept
			: m_components{ allocator }
		{
		}

		entity_data(entity_data&&) = default;
		entity_data& operator=(entity_data&&) = default;

		template <typename T>
		void push_component(T* ptr) noexcept
		{
			auto& components = m_components[type_id<T>::get_id()];
			
			if (components.empty())
			{
				auto alloc = mem::pool::allocator<std::byte*>{ m_components.get_allocator() };
				components = mem::vector<std::byte*>{ alloc };
			}

			m_components[type_id<T>::get_id()].push_back(reinterpret_cast<std::byte*>(ptr));
		}

		template <typename T>
		void pop_component(T* ptr) noexcept
		{
			auto found = std::lower_bound
				m_components.at(type_id<T>::get_id()).erase();
		}

		template <typename... TArgs>
		bool has_component() const noexcept
		{
			return has_component<std::tuple<TArgs...>>(std::make_index_sequence<sizeof...(TArgs)>{});
		}

		template <typename TTuple, std::uint64_t... TSequence>
		void has_component_impl(std::index_sequence<TSequence...>) const noexcept
		{
			return (... && (m_components.find(type_id<std::tuple_element_t<TSequence, TTuple>>::get_id()) != m_components.cend()));
		}

		bool has_component(type_id_t id) const noexcept
		{
			return m_components.find(id) != m_components.cend();
		}

		vector<type_id_t> get_component_ids() const noexcept
		{
			auto result = vector<type_id_t>{};
			result.reserve(m_components.size());

			for (auto const& pair : m_components)
				result.push_back(pair.first);

			return result;
		}

		template <typename T>
		T& get_component(std::uint64_t index) noexcept
		{
			return *reinterpret_cast<T*>(m_components.at(type_id<T>::get_id())[index]);
		}

		template <typename T>
		T const& get_component(std::uint64_t index) const noexcept
		{
			return *reinterpret_cast<T*>(m_components.at(type_id<T>::get_id())[index]);
		}

		template <typename T>
		std::uint64_t size() const noexcept
		{
			return m_components.at(type_id<T>::get_id()).size();
		}


	private:
		mem::dictionary<type_id_t, mem::vector<std::byte*>> m_components;

	};
}

	class entity
	{
	public:
		entity(impl::entity_data* data = nullptr) noexcept
			: m_data{ data }
		{
		}

		template <typename T>
		bool has_component() const noexcept
		{
			return m_data->has_component<T>();
		}

		bool has_component(type_id_t id) const noexcept
		{
			return m_data->has_component(id);
		}

		vector<type_id_t> get_component_ids() const noexcept
		{
			return m_data->get_component_ids();
		}

		template <typename T>
		T& get_component(std::uint64_t index) noexcept
		{
			return m_data->get_component<T>(index);
		}

		template <typename T>
		T const& get_component(std::uint64_t index) const noexcept
		{
			return m_data->get_component<T>(index);
		}

		template <typename T>
		std::uint64_t size() const noexcept
		{
			return m_data->size<T>();
		}

	private:
		friend class organizer;

	private:
		impl::entity_data* m_data;
	};
}
}