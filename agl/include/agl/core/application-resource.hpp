#pragma once
#include "agl/util/typeid.hpp"
#include "agl/util/set.hpp"

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
}
