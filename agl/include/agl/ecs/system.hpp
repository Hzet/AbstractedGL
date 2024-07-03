#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include "agl/util/typeid.hpp"
#include "agl/memory/set.hpp"

namespace agl
{
class application;

namespace ecs
{
class entity;
class organizer;

enum stage : std::uint64_t 
{
	PRE_RENDER,
	RENDER,
	POST_RENDER,
};

enum signal_message_type
{
	COMPONENT_ATTACH,
	COMPONENT_DETACH,
};

struct signal
{
	signal_message_type message;
	type_id_t           type_id;
};

constexpr bool operator<(signal const& lhs, signal const& rhs)
{
	if (lhs.type_id == rhs.type_id)
		return lhs.message < rhs.message;
	return lhs.type_id < rhs.type_id;
}

class system_base
{
public:
	                   system_base();
	                   system_base(organizer* organizer, type_id_t id, ecs::stage stage);
	                   system_base(system_base&& other);
	                   system_base& operator=(system_base&& other);
	virtual            ~system_base() = default;
	type_id_t          get_type_id() const;
	organizer*         get_organizer();
	bool               is_signal_registered(type_id_t type_id, signal_message_type message) const;
	virtual void       on_attach(application*) = 0;
	virtual void       on_component_attach(entity* e, type_id_t type_id, std::uint64_t index);
	virtual void       on_component_detach(entity* e, type_id_t type_id, std::uint64_t index);
	virtual void       on_detach(application*) = 0;
	virtual void       on_update(application*) = 0;
	stage              stage() const;
	void               stage(ecs::stage s);

protected:
	void register_signal(type_id_t type_id, signal_message_type message);

private:
	friend class organizer;

private:
	void set_organizer(organizer* org);

private:
	organizer*       m_organizer;
	mem::set<signal> m_signals;
	ecs::stage       m_stage;
	type_id_t        m_type_id;
};

template <typename T>
class system
	: public system_base
{
public:
	        system();
	        system(system&& other);
	        system& operator=(system&& other);
	        system(organizer* organizer, ecs::stage stage);
	virtual ~system() = default;
};

template <typename T>
system<T>::system()
	: system_base{}
{
}
template <typename T>
system<T>::system(system&& other)
	: system_base{ std::move(other) }
{
}
template <typename T>
system<T>& system<T>::operator=(system&& other)
{
	this->system_base::operator=(std::move(other));
	return *this;
}
template <typename T>
system<T>::system(organizer* organizer, ecs::stage stage)
	: system_base{ organizer, type_id<T>::get_id(), stage }
{
}
}
}