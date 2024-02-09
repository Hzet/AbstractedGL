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
	: resource<logger>{ }
	, m_cond_var{ nullptr }
	, m_mutex{ nullptr }
	, m_messages_count{ 0 }
{
}
logger::logger(logger&& other) noexcept
	: resource<logger>{ std::move(other) }
	, m_messages_count{ other.m_messages_count.load() }
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
bool logger::is_active() const noexcept
{
	AGL_ASSERT(m_thread != nullptr, "invalid thread object");
	return m_thread->is_valid() && m_thread->is_running();
}
std::string logger::get_date() noexcept
{
	using namespace date;
	using namespace std::chrono;
	auto const now = floor<date::days>(system_clock::now());
	auto ss = std::stringstream{};
	ss << year_month_day{ now } << ' ' << make_time(system_clock::now() - now);
	return ss.str();
}
void logger::on_attach(application* app) noexcept
{
	auto logger_thread = [&]
		{
			AGL_ASSERT(m_mutex != nullptr, "invalid mutex");
			AGL_ASSERT(m_cond_var != nullptr, "invalid cond_var");

			while (true)
			{
				std::unique_lock<std::mutex> lock{ *m_mutex };
				auto stop_waiting = [&]
					{
						return m_messages_count > 0 || m_thread->should_close();
					};
				m_cond_var->wait(lock, stop_waiting);
				if (m_messages_count > 0)
				{
					auto const messages = m_messages;
					m_messages.clear();
					m_messages_count = 0;
					lock.unlock();

					for (auto& msg : messages)
					{
						auto& stream = *m_loggers[msg.destination].m_stream;
						stream << msg.message << "\n";
					}
				}
				else if (m_thread->should_close())
				{
					break;
				}
			}
		};

	// init loggers
	for (auto i = 0; i < static_cast<std::uint64_t>(SIZE); ++i)
		m_loggers.push_back({ get_logger_name(static_cast<instance_index>(i)), &std::cout });

	// init mutex and cond_var
	auto& threads = app->get_resource<agl::threads>();
	m_thread = &threads.new_thread();
	m_cond_var = &threads.new_condition_variable();
	m_mutex = &threads.new_mutex();
	m_thread->start_no_log(app, "logger", logger_thread);

	info("Logger: OK");
}
void logger::on_detach(application* app) noexcept
{
	info("Logger: OFF");
	auto& threads = app->get_resource<agl::threads>();
	threads.delete_thread(*m_thread);
	threads.delete_condition_variable(*m_cond_var);
	threads.delete_mutex(*m_mutex);
}
void logger::on_update(application* app) noexcept
{
}
}