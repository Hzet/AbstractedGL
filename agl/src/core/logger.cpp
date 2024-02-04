#include "agl/core/threads.hpp"

namespace agl
{
const char* logger::get_logger_name(instance_index index) noexcept
{
	switch (index)
	{
	case DEBUG: return "DEBUG";
	case ERROR: return "ERROR";
	case INFO: return "INFO";
	case TRACE: return "TRACE";
	case WARNING: return "WARNING";
	}
	AGL_ASSERT(false, "invalid logger index");
	return "UNKNOWN";
}
logger::logger() noexcept
	: application_resource{ type_id<logger>::get_id() }
	, m_cond_var{ nullptr }
	, m_mutex{ nullptr }
{
}
logger::logger(logger&& other) noexcept
	: application_resource{ std::move(other) }
{
	if (this == &other)
		return;

	if (other.m_mutex != nullptr)
		other.m_mutex->lock();

	m_messages = std::move(other.m_messages);
	m_cond_var = other.m_cond_var;
	m_loggers = std::move(other.m_loggers);
	m_thread = other.m_thread;
	m_mutex = other.m_mutex;

	if (m_mutex != nullptr)
		m_mutex->unlock();
}
logger::~logger() noexcept
{
}
void logger::on_attach(application* app) noexcept
{
	auto logger_thread = [&]
		{
			AGL_ASSERT(m_mutex != nullptr, "invalid mutex");
			AGL_ASSERT(m_cond_var != nullptr, "invalid cond_var");

			std::unique_lock<std::mutex> lock{ *m_mutex };
			auto fun = [&]
				{
					return !m_messages.empty();
				};
			m_cond_var->wait(lock, fun);
			auto messages = m_messages;
			m_messages.clear();
			lock.unlock();

			for (auto& msg : messages)
			{
				auto& stream = *m_loggers[msg.destination].m_stream;
				stream << msg.message << "\n";
			}
		};

	// init loggers
	for (auto i = 0; i < static_cast<std::uint64_t>(SIZE); ++i)
		m_loggers.push_back({ get_logger_name(static_cast<instance_index>(i)), &std::cout });

	// init mutex and cond_var
	auto& threads = app->get_resource<agl::threads>();
	m_thread = &threads.add_thread();
	m_cond_var = &m_thread->push_condition_variable();
	m_mutex = &m_thread->push_mutex();
	m_thread->start_no_log(app, "logger", logger_thread);

	info("Logger: OK");
}
void logger::on_detach(application* app) noexcept
{
	info("Logger: OFF");
	auto& threads = app->get_resource<agl::threads>();
	threads.remove_thread(*m_thread);
}
void logger::on_update(application* app) noexcept
{
}
}