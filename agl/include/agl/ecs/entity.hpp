#pragma once
#include "agl/util/memory/containers.hpp"
#include "agl/util/typeid.hpp"

namespace agl
{
namespace ecs
{
namespace impl
{
	class entity_data
	{
	public:
		entity_data(mem::pool::allocator<std::pair<std::uint64_t, mem::vector<std::byte*>>> const& allocator = {}) noexcept
			: m_components{ allocator }
		{
		}

		template <typename T>
		void push_component(T* ptr) noexcept
		{
			if (m_components.find(type_id<T>::get_id()) == m_components.cend())
				m_components[type_id<T>::get_id()] = mem::vector<std::byte*>(m_components.get_allocator());

			m_components[type_id<T>::get_id()].push_back(ptr);
		}

		template <typename T>
		void pop_component(T* ptr) noexcept
		{
			auto found = std::lower_bound
				m_components.at(type_id<T>::get_id()).erase();
		}

		template <typename T>
		bool has_component() const noexcept
		{
			return m_components.find(type_id<T>::get_id()) != m_components.cend();
		}

		bool has_component(std::uint64_t id) const noexcept
		{
			return m_components.find(id) != m_components.cend();
		}

		std::vector<std::uint64_t> get_component_ids() const noexcept
		{
			auto result = std::vector<std::uint64_t>{};
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
		mem::unordered_map<std::uint64_t, mem::vector<std::byte*>> m_components;

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

		bool has_component(std::uint64_t id) const noexcept
		{
			return m_data->has_component(id);
		}

		std::vector<std::uint64_t> get_component_ids() const noexcept
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