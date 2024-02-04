#pragma once
#include "agl/util/async.hpp"
#include "agl/core/logger.hpp"

namespace agl
{
class threads;

class thread
{
public:
	thread() noexcept;
	virtual ~thread()
	{
		close();
	}
	[[nodiscard]] mutex& push_mutex()
	{
		AGL_ASSERT(!is_running(), "all mutexes must be initialized before the thread starts");
		auto ptr = make_unique<mutex>();
		ptr->m_id = m_mutexes.size();
		return **m_mutexes.insert(m_mutexes.cend(), std::move(ptr));
	}
	[[nodiscard]] condition_variable& push_condition_variable()
	{
		AGL_ASSERT(!is_running(), "all condition variables must be initialized before the thread starts");
		auto ptr = make_unique<condition_variable>();
		ptr->m_id = m_cond_vars.size();
		return **m_cond_vars.insert(m_cond_vars.cend(), std::move(ptr));
	}
	void close()
	{
		if (!is_running())
			return;

		std::unique_lock<std::mutex> lock{ *m_my_mutex };
		m_should_close = true;
		m_my_cond_var->notify_one();

		if (m_thread->joinable())
			m_thread->join();
	}
	bool is_running() const noexcept
	{
		return m_is_running;
	}
	bool is_valid() const noexcept;
	std::thread::id id() const noexcept
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_thread->get_id();
	}

	template <typename TFun, typename... TArgs>
	void start(application* app, std::string const& name, TFun fun, TArgs&&... args) noexcept
	{
		start_impl(app, true, name, fun, std::forward<TArgs>(args)...);
	}
	template <typename TFun, typename... TArgs>
	void start_no_log(application* app, std::string const& name, TFun fun, TArgs&&... args) noexcept
	{
		start_impl(app, false, name, fun, std::forward<TArgs>(args)...);

	}
private:
	template <typename TFun, typename... TArgs>
	void start_impl(application* app, bool log, std::string const& name, TFun fun, TArgs&&... args) noexcept
	{
		AGL_ASSERT(!is_running(), "thread already running");

		m_my_cond_var = &push_condition_variable();
		m_my_mutex = &push_mutex();
		m_name = name;
		m_is_running = true;

		AGL_ASSERT(is_valid(), "invalid object");

		m_thread = make_unique<std::thread>(std::thread{ &thread::template thread_function<TFun, TArgs...>, this, app, log, fun, std::forward<TArgs>(args)... });
	}

	template <typename TFun, typename... TArgs>
	void thread_function(application* app, bool log, TFun&& fun, TArgs&&... args) noexcept
	{
		if (log)
			app->get_resource<agl::logger>().debug("Starting thread \"{}\" - [{}]", m_name, std::this_thread::get_id());

		while (true)
		{
			fun(std::forward<TArgs>(args)...);
			std::unique_lock<std::mutex> lock{ *m_my_mutex };
			m_my_cond_var->wait(lock, [&] { return !m_should_close; });
			if (m_should_close)
			{
				m_is_running = false;
				break;
			}
		}

		if (log)
			app->get_resource<agl::logger>().debug("Ending thread \"{}\" - [{}]", m_name, std::this_thread::get_id());
	}

protected:
	unique_ptr<std::thread> m_thread;

private:
	friend class ::agl::threads;

private:
	vector<unique_ptr<condition_variable>> m_cond_vars;
	bool m_is_running;
	bool m_should_close;
	vector<unique_ptr<mutex>> m_mutexes;
	std::string m_name;
	mutex* m_my_mutex;
	condition_variable* m_my_cond_var;
	std::uint64_t m_internal_id;
};
}