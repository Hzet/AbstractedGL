#include "agl/core/layer.hpp"
#include "agl/core/logger.hpp"

namespace agl
{
layer_base::layer_base()
{
}
layer_base::layer_base(type_id_t type_id, std::string const& name)
	: m_name{ name }
	, m_type_id{ type_id }
{
}
layer_base::layer_base(layer_base&& other)
	: m_name{ other.m_name }
	, m_type_id{ other.m_type_id }
{
}
layer_base& layer_base::operator=(layer_base&& other)
{
	m_name = other.m_name;
	m_type_id = other.m_type_id;
	return *this;
}
layer_base::~layer_base()
{
}
layers& layer_base::get_layers()
{
	return *m_layers;
}
std::string const& layer_base::get_name() const
{
	return m_name;
}
type_id_t layer_base::get_type_id() const
{
	return m_type_id;
}
bool layers::has_layer(layer_base& layer) const
{
	for (auto& ptr : m_layers)
		if (ptr->get_type_id() == layer.get_type_id())
			return true;
	return false;
}
void layers::push_layer(application* app, unique_ptr<layer_base> layer)
{
	m_layers.emplace_front(std::move(layer));
	m_layers.front()->on_attach(app);
}
void layers::pop_layer(application* app)
{
	m_layers.front()->on_detach(app);
	m_layers.pop_front();
}
void layers::on_attach(application* app)
{
	auto& logger = app->get_resource<agl::logger>();
	logger.debug("Layers: ON");
}
void layers::on_detach(application* app)
{
	while (!m_layers.empty())
		pop_layer(app);

	auto& logger = app->get_resource<agl::logger>();
	logger.debug("Layers: OFF");
}
void layers::on_update(application* app)
{
	if (m_layers.empty())
		return;

	auto& layer = *m_layers.front();
	layer.on_update(app);

	if (m_pop_layer)
		pop_layer(app);

	for (auto& ptr : m_layers_to_push)
		push_layer(app, std::move(ptr));

	m_layers_to_push.clear();
}
}