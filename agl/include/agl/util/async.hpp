#pragma once
#include <condition_variable>
#include "agl/core/debug.hpp"

namespace agl
{
/**
 * @brief 
 * std::mutex wrapper.
 */
class mutex
{
public:
	mutex();
	mutex(mutex const&) = delete;
	mutex& operator=(mutex const&) = delete;
	void mutex::lock()
	{
		AGL_ASSERT(is_valid(), "invalid object");
		m_mutex.lock();
	}
	void unlock()
	{
		AGL_ASSERT(is_valid(), "invalid object");
		m_mutex.unlock();
	}
	bool try_lock()
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_mutex.try_lock();
	}
	std::uint64_t id() const
	{
		return m_id;
	}
	bool is_valid() const;
	operator std::mutex& ()
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_mutex;
	}
	operator std::mutex const& () const
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_mutex;
	}

private:
	friend class threads;

private:
	std::uint64_t m_id;
	std::mutex m_mutex;
};

/**
 * @brief 
 * std::condition_variable wrapper
 */
class condition_variable
{
public:
	condition_variable();
	condition_variable(condition_variable const&) = delete;
	condition_variable& operator=(condition_variable const&) = delete;
	std::uint64_t id() const
	{
		return m_id;
	}
	bool is_valid() const;
	void notify_one()
	{
		AGL_ASSERT(is_valid(), "invalid object");
		m_cond_var.notify_one();
	}
	void notify_all()
	{
		AGL_ASSERT(is_valid(), "invalid object");
		m_cond_var.notify_all();
	}

	template <typename TFun>
	void wait(std::unique_lock<std::mutex>& lock, TFun stop_waiting)
	{
		AGL_ASSERT(is_valid(), "invalid object");
		m_cond_var.wait(lock, stop_waiting);
	}
	void wait(std::unique_lock<std::mutex>& lock)
	{
		AGL_ASSERT(is_valid(), "invalid object");
		m_cond_var.wait(lock);
	}

	template <typename TRep, typename TPeriod>
	std::cv_status wait_for(std::unique_lock<std::mutex>& lock, std::chrono::duration<TRep, TPeriod> const& rel_time)
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_cond_var.wait_for(lock, rel_time);
	}
	template <typename TRep, typename TPeriod, typename TFun>
	bool wait_for(std::unique_lock<std::mutex>& lock, std::chrono::duration<TRep, TPeriod> const& rel_time, TFun stop_waiting)
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_cond_var.wait_for(lock, rel_time, stop_waiting);
	}

	template <typename TClock, typename TDuration>
	std::cv_status wait_until(std::unique_lock<std::mutex>& lock, std::chrono::time_point<TClock, TDuration> const& timeout_time)
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_cond_var.wait_until(lock, timeout_time);
	}

	template <typename TClock, typename TDuration, typename TFun>
	bool wait_until(std::unique_lock<std::mutex>& lock, std::chrono::time_point<TClock, TDuration> const& timeout_time, TFun fun)
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_cond_var.wait_until(lock, timeout_time, fun);
	}
	operator std::condition_variable& ()
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_cond_var;
	}
	operator std::condition_variable const& () const
	{
		AGL_ASSERT(is_valid(), "invalid object");
		return m_cond_var;
	}

private:
	friend class threads;

private:
	std::uint64_t m_id;
	std::condition_variable m_cond_var;
};
}