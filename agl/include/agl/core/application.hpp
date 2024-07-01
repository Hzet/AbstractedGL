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
	resource_base(resource_base&&) = delete;
	resource_base(resource_base const&) = delete;
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
		bool is_good;
	};

public:
	                                  application() = default;
	                                  application(application&&) = delete;
	                                  application(application const&) = delete;
	                                  ~application();
	void                              add_resource(unique_ptr<resource_base> resource);
	void                              close();
	void                              destroy();
	template <typename T>             
	T*                                get_resource();
	resource_base*                    get_resource(type_id_t type);
	std::string                       get_current_path() const;
	bool                              is_good() const;
	bool                              is_open() const;
	void                              init();
	template <typename T>             
	void                              remove_resource();
	void                              remove_resource(type_id_t type);
					                  
private:			                  
	friend int                        process_application(unique_ptr<application>&);
					                  
private:			                  
	void                              run();

private:
	std::mutex                        m_mutex;
	properties                        m_properties;
	vector<unique_ptr<resource_base>> m_resources;
};

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