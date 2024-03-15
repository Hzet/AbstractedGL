#pragma once
#include <cstdint>
#include <string>
#include "agl/util/typeid.hpp"
#include "agl/memory/set.hpp"

namespace agl
{
class application;

namespace ecs
{
class organizer;

enum stage : std::uint64_t 
{
	PRE_RENDER,
	RENDER,
	POST_RENDER,
};

struct signal
{
	std::uint64_t id;
	bool value;
};

struct signal_comp
{
	bool operator()(signal const& lhs, std::uint64_t id) const
	{
		return lhs.id < id;
	}
	bool operator()(std::uint64_t id, signal const& rhs) const
	{
		return id < rhs.id;
	}
	bool operator()(signal const& lhs, signal const& rhs) const
	{
		return lhs.id < rhs.id;
	}
};

class system_base
{
public:
	system_base();
	// TODO: get rid of name parameter and get the name of the class from type_id_t
	system_base(type_id_t id, std::string const& name, ecs::stage stage);
	system_base(system_base&& other);
	system_base& operator=(system_base&& other);
	virtual ~system_base() = default;

	type_id_t id() const;
	std::string const& name() const;
	void name(std::string const& name);
	virtual void on_attach(application*) = 0;
	virtual void on_detach(application*) = 0;
	virtual void on_update(application*) = 0;
	stage stage() const;
	void stage(ecs::stage s);
	bool read_signal(std::uint64_t id);
	void set_signal(std::uint64_t id, bool value);
	organizer& get_organizer();

protected:
	void create_signal(std::uint64_t id, bool start_value);

private:
	friend class organizer;

private:
	void set_organizer(organizer* org);

private:
	type_id_t m_id;
	std::string m_name;
	organizer* m_organizer;
	mem::set<signal, signal_comp> m_signals;
	ecs::stage m_stage;
};

template <typename T>
class system
	: public system_base
{
public:
	system()
		: system_base()
	{
	}
	system(system&& other)
		: system_base{ std::move(other) }
	{
	}
	system& operator=(system&& other)
	{
		this->system_base::operator=(std::move(other));
		return *this;
	}
	system(ecs::stage stage)
		: system_base{ type_id<T>::get_id(), std::string{ type_id<T>::get_name() }, stage
}
	{
	}
	virtual ~system() = default;
};
}
}