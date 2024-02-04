#pragma once
#include <string>
#include "agl/core/application.hpp"
#include "agl/util/thread.hpp"

namespace agl
{
class threads final
	: public application_resource
{
public:
	static std::uint64_t invalid_id() noexcept
	{
		return std::numeric_limits<std::uint64_t>::max();
	}

public:
	threads() noexcept
		: application_resource{ type_id<threads>::get_id() }
	{
	}
	threads(threads&& other) noexcept = default;
	~threads() noexcept
	{
		while (!m_threads.empty())
			m_threads.erase(m_threads.cend() - 1);
	}
	[[nodiscard]] thread& add_thread() noexcept
	{
		auto ptr = make_unique<thread>();
		ptr->m_internal_id = m_threads.size();
		return **m_threads.insert(m_threads.cend(), std::move(ptr));
	}
	void remove_thread(thread const& thread) noexcept
	{
		m_threads.erase(m_threads.cbegin() + thread.m_internal_id);
	}

private:
	virtual void on_attach(application* app) noexcept 
	{
		m_app = app;
	};
	virtual void on_update(application*) noexcept {};
	virtual void on_detach(application*) noexcept {};

private:
	application* m_app;
	vector<unique_ptr<std::condition_variable>> m_cond_vars;
	vector<unique_ptr<std::mutex>> m_mutexes;
	vector<unique_ptr<thread>> m_threads;
};
}