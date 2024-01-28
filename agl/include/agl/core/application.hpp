#pragma once
#include "agl/util/dictionary.hpp"
#include "agl/util/typeid.hpp"
#include "agl/util/unique-ptr.hpp"

int main(int, char**);

namespace agl
{
class application;

class application_resource
{
public:
	application_resource(type_id_t id = {}) noexcept
		: m_id{ id }
	{
	}
	application_resource(application_resource&&) noexcept = default;
	application_resource(application_resource const&) noexcept = delete;
	application_resource& operator=(application_resource&&) noexcept = default;
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

extern std::unique_ptr<application> create_application();

class application
{
public:
	struct properties
	{
		bool is_open;
	};

public:
	application() = default;
	application(application&&) = default;
	application(application const&) = delete;
	application& operator=(application&&) = default;
	application& operator=(application const&) = delete;

	properties const& get_properties() const;
	void close();
	virtual void init();

	template <typename T>
	bool has_resource() noexcept
	{
		return m_resources.find(type_id<T>::get_id()) != m_resources.end();
	}

	template <typename T>
	void add_resource(T&& resource) noexcept
	{
		using type = std::remove_cv_t<std::remove_reference_t<T>>; // for allocator needs
		AGL_ASSERT(!has_resource<type>(), "resource already present");

		auto& r = m_resources[type_id<type>::get_id()];
		r = make_unique<application_resource>(std::forward<type>(resource));
		r->on_attach(this);
	}

	template <typename T>
	void remove_resource() noexcept
	{
		AGL_ASSERT(has_resource<T>(), "invalid resource");

		auto found = m_resources.find(type_id<T>::get_id());
		found->second->on_detach(this);
		m_resources.erase(found);
	}

	template <typename T>
	T& get_resource() noexcept
	{
		AGL_ASSERT(has_resource<T>(), "invalid resource");

		auto found = m_resources.find(type_id<T>::get_id());
		return *reinterpret_cast<T*>(found->second.get());
	}

private:
	friend int ::main(int, char**);

private:
	void run();

private:
	properties m_properties;
	dictionary<type_id_t, unique_ptr<application_resource>> m_resources;
};
}