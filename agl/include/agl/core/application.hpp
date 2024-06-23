#pragma once
#include <mutex>
#include "agl/dictionary.hpp"
#include "agl/util/typeid.hpp"
#include "agl/unique-ptr.hpp"

int main(int, char**);

namespace agl
{
class application;

class resource_base
{
public:
	resource_base(type_id_t id = {});
	resource_base(resource_base&&) = default;
	resource_base& operator=(resource_base&&) = default;
	virtual ~resource_base() = default;
	virtual void on_attach(application*) = 0;
	virtual void on_detach(application*) = 0;
	virtual void on_update(application*) = 0;
	type_id_t type() const;

private:
	type_id_t m_id;
};

template <typename T>
class resource
	: public resource_base
{
public:
	using resource_base::resource_base;
	resource()
		: resource_base{ type_id<T>::get_id() }
	{
	}
};

extern unique_ptr<application> create_application();

class application
{
public:
	struct properties
	{
		bool is_open;
	};

public:
	application() = default;
	application(application&&) = delete;
	application(application const&) = delete;
	application& operator=(application&&) = delete;
	application& operator=(application const&) = delete;
	~application();
	
	void add_resource(unique_ptr<resource_base> resource);
	bool has_resource(type_id_t type);
	void remove_resource(type_id_t type);
	template <typename T>
	T* get_resource();
	resource_base* get_resource(type_id_t type);
	void close();
	void destroy();

	properties const& get_properties() const;

	std::string get_current_path() const;

	template <typename T>
	bool has_resource();

	void init();
	bool good() const;

	template <typename T>
	void remove_resource();

	void use_opengl();

private:
	friend int process_application(unique_ptr<application>&);

private:
	void run();
	void deinit_opengl();

private:
	bool m_good;
	std::mutex m_mutex;
	properties m_properties;
	dictionary<type_id_t, unique_ptr<resource_base>> m_resources;
	vector<type_id_t> m_resources_order;
};

template <typename T>
bool application::has_resource()
{
	return has_resource(type_id<T>::get_id());
}
template <typename T>
void application::remove_resource()
{
	remove_resource(type_id<T>::get_id());
}

template <typename T>
T* application::get_resource()
{
	auto* ptr = get_resource(type_id<T>::get_id());

	auto* cast = reinterpret_cast<T*>(ptr);

	AGL_ASSERT(cast != nullptr, "invalid cast!");

	return cast;
}

}