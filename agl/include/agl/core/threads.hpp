#pragma once
#include <string>
#include "agl/core/application.hpp"
#include "agl/util/thread.hpp"

namespace agl
{
class threads final
	: public resource<threads>
{
public:
	static std::uint64_t invalid_id() noexcept
	{
		return std::numeric_limits<std::uint64_t>::max();
	}

public:
	threads() noexcept
		: resource<threads>{ type_id<threads>::get_id() }
	{
	}
	threads(threads&& other) noexcept = default;
	[[nodiscard]] thread& new_thread()
	{
		auto ptr = make_unique<thread>();
		ptr->m_internal_id = m_threads.size();
		return **m_threads.insert(m_threads.cend(), std::move(ptr));
	}
	void delete_thread(thread const& thread)
	{
		m_threads.erase(m_threads.cbegin() + thread.m_internal_id);
	}
	[[nodiscard]] mutex& new_mutex()
	{
		auto ptr = make_unique<mutex>();
		ptr->m_id = m_mutexes.size();
		return **m_mutexes.insert(m_mutexes.cend(), std::move(ptr));
	}
	void delete_mutex(mutex const& m)
	{
		AGL_ASSERT(m.is_valid(), "invalid mutex");
		m_mutexes.erase(m_mutexes.cbegin() + m.id());
	}
	[[nodiscard]] condition_variable& new_condition_variable()
	{
		auto ptr = make_unique<condition_variable>();
		ptr->m_id = m_cond_vars.size();
		return **m_cond_vars.insert(m_cond_vars.cend(), std::move(ptr));
	}
	void delete_condition_variable(condition_variable& cond_var)
	{
		AGL_ASSERT(cond_var.is_valid(), "invalid condition variable");
		m_cond_vars.erase(m_cond_vars.cbegin() + cond_var.id());
	}
private:
	virtual void on_attach(application* app) noexcept 
	{
	};
	virtual void on_update(application*) noexcept {};
	virtual void on_detach(application*) noexcept 
	{
		while (!m_threads.empty())
			m_threads.erase(m_threads.cend() - 1);
	};

private:
	vector<unique_ptr<condition_variable>> m_cond_vars;
	vector<unique_ptr<mutex>> m_mutexes;
	vector<unique_ptr<thread>> m_threads;
};
}