#pragma once
#include "agl/core/application.hpp"

namespace agl
{
class application;
class layers;

class layer_base
{
public:
	layer_base();
	layer_base(type_id_t type_id, std::string const& name);
	layer_base(layer_base&& other);
	layer_base& operator=(layer_base&& other);
	virtual ~layer_base();

	virtual void on_attach(application*) = 0;
	virtual void on_detach(application*) = 0;
	virtual void on_update(application*) = 0;

	std::string const& get_name() const;
	type_id_t get_type_id() const;

protected:
	layers& get_layers();

private:
	layers* m_layers;
	type_id_t m_type_id;
	std::string m_name;
};

template <typename T>
class layer
	: public layer_base
{
public:
	layer()
		: layer_base{ type_id<T>::get_id(), std::string{ type_id<T>::get_name() } }
	{
	}
	layer(layer&& other)
		: layer_base(std::move(other))
	{
	}
	layer& operator=(layer&& other)
	{
		return this->layer_base::operator=(std::move(other));
	}
	virtual ~layer()
	{
	}
};

class layers
	: public resource<layers>
{
public:
	bool has_layer(layer_base& layer) const;
	void push_layer(unique_ptr<layer_base> layer);
	void pop_layer();
	
private:
	virtual void on_attach(application* app) override;
	virtual void on_detach(application* app) override;
	virtual void on_update(application* app) override;
	void push_layer(application* app, unique_ptr<layer_base> layer);
	void pop_layer(application* app);

private:
	vector<unique_ptr<layer_base>> m_layers;
	vector<unique_ptr<layer_base>> m_layers_to_push;
	bool m_pop_layer;
};
}