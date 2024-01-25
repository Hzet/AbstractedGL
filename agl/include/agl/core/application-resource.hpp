#pragma once
#include "agl/util/typeid.hpp"
#include "agl/util/set.hpp"
#include "agl/util/memory/unique-ptr.hpp"

namespace agl
{
class application;

namespace impl
{
class application_resource
{
public:
	application_resource(type_id_t id = {}) noexcept
		: m_id{ id }
	{
	}
	application_resource(application_resource const&) noexcept = delete;
	application_resource& operator=(application_resource const&) noexcept = delete;
	virtual ~application_resource() noexcept = default;
	type_id_t type() const noexcept
	{
		return m_id;
	}
	virtual void on_attach(application*) noexcept = 0;
	virtual void on_update(application*) noexcept = 0;
	virtual void on_detach(application*) noexcept = 0;

private:
	type_id_t m_id;
};

class resources
{
private:
	struct comp
	{
		using const_reference = unique_ptr<application_resource>;
		bool operator()(type_id_t lhs, const_reference rhs) const noexcept
		{
			return lhs < rhs->type();
		}
		bool operator()(const_reference lhs, const_reference rhs) const noexcept
		{
			return lhs->type() < rhs->type();
		}
	};

	using resources_set = set<unique_ptr<application_resource>, comp>;

public:
	using value_type = typename resources_set::value_type;
	using pointer = typename resources_set::pointer;
	using const_pointer = typename resources_set::const_pointer;
	using reference = typename resources_set::reference;
	using const_reference = typename resources_set::const_reference;
	using size_type = typename resources_set::size_type;
	using iterator = typename resources_set::iterator;
	template <typename TApp>
	using is_app = std::enable_if_t<std::is_same_v<TApp, application>>;
	template <typename T> 
	using is_resource = std::enable_if_t<std::is_base_of_v<application_resource, T>>;
public:
	iterator begin() const noexcept
	{
		return m_resources.begin();
	}
	iterator end() const noexcept
	{
		return m_resources.end();
	}
	template <typename TApp, typename T, typename = is_app<TApp>, typename = is_resource<T>>
	void add_resource(TApp* app) noexcept
	{
		auto it = m_resources.emplace(make_unique<application_resource>(T{}));
		(*it)->on_attach(app);
	}
	template <typename TApp, typename T, typename = is_app<TApp>, typename = is_resource<T>>
	void remove_resource(TApp* app) noexcept
	{
		auto it = m_resources.at(type);
		(*it)->on_detach(app);
		m_resources.erase(it);
	}
	template <typename T>
	iterator get_resource() noexcept
	{
		return m_resources.find(type_id<T>::get_id());
	}
	template <typename TApp, typename = is_app<TApp>>
	void update(TApp* app) noexcept
	{
		for (auto& r : m_resources)
			r->on_update(app);
	}

private:
	resources_set m_resources;
};
}
}