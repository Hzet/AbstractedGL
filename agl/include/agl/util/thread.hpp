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
	thread(thread&&) noexcept = default;
	thread& operator=(thread&&) noexcept = default;
	virtual ~thread()
	{
		close();
	}
	bool should_close() const noexcept
	{
		return m_should_close;
	}
	void close()
	{
		if (!is_running())
			return;

		m_should_close = true;
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

		fun(std::forward<TArgs>(args)...);

		if (log)
			app->get_resource<agl::logger>().debug("Ending thread \"{}\" - [{}]", m_name, std::this_thread::get_id());
	}

protected:
	unique_ptr<std::thread> m_thread;

private:
	friend class ::agl::threads;

private:
	std::atomic<bool> m_is_running;
	std::atomic<bool> m_should_close;
	std::string m_name;
	std::uint64_t m_internal_id;
};
}