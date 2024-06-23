#include "agl/core/logger.hpp"

namespace agl
{
const char* logger::get_logger_name(instance_index index)
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
static auto const argument_pattern         = std::regex{ "\\{[0-9]*\\}" };
static auto const argument_empty_pattern   = std::regex{ "\\{\\}" };
static auto const argument_indexed_pattern = std::regex{ "\\{[0-9]+\\}" };
static auto const escape_lbracket_pattern  = std::regex{ "\\\\\\{" };
static auto const escape_rbracket_pattern  = std::regex{ "\\\\\\}" };

std::string logger::combine_message_impl(std::string const& message, vector<std::string> const& parsed)
{
	auto it = std::sregex_iterator{ message.cbegin(), message.cend(), argument_pattern };
	auto const end = std::sregex_iterator{};
	auto arg_index = std::uint64_t{};
	auto str = std::string{};
	auto offset = std::int64_t{};
	auto output_message = message;

	for (it; it != end; ++it)
	{
		auto const found_idx = it->position();
		auto const found_str = it->str();
		if (std::regex_search(it->str(), argument_empty_pattern))
		{
			AGL_CORE_ASSERT(arg_index < parsed.size(), "too many arguments in provided string");
			str = parsed[arg_index];
			++arg_index;
		}
		else if (std::regex_search(it->str(), argument_indexed_pattern))
		{
			str = it->str().substr(1, it->str().size() - 2);
			auto index = std::stoi(str);
			AGL_CORE_ASSERT(index <= parsed.size(), "argument index out of bounds");
			str = parsed[index];
		}
		else
		{
			continue;
		}

		output_message.erase(it->position() + offset, it->str().size());
		output_message.insert(it->position() + offset, str);
		offset += str.size() - it->str().size();
	}
	output_message = std::regex_replace(output_message, escape_lbracket_pattern, "{");
	output_message = std::regex_replace(output_message, escape_rbracket_pattern, "}");
	return output_message;
}
logger::logger()
	: resource<logger>{ }
	, m_messages_count{ 0 }
	, m_should_close{ false }
	, m_thread_finished{ false }
	, m_thread_started{ false }
{
}
bool logger::is_active() const
{
	return m_thread_started;
}
std::string logger::get_date()
{
	using namespace date;
	using namespace std::chrono;
	auto const now = floor<date::days>(system_clock::now());
	auto ss = std::stringstream{};
	ss << year_month_day{ now } << ' ' << make_time(system_clock::now() - now);
	return ss.str();
}
void logger::on_attach(application* app)
{
	auto logger_thread = [&]
		{
			this->debug("Logger: using thread {}", std::this_thread::get_id());
			m_thread_started = true;

			while (true)
			{
				std::unique_lock<std::mutex> lock{ m_mutex };
				auto stop_waiting = [&]
					{
						return m_messages_count > 0 || m_should_close;
					};
				auto log_messages = [&]
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
					};

				m_cond_var.wait(lock, stop_waiting);
				if (m_messages_count > 0)
				{
					log_messages();
				}
				else if (m_should_close)
				{
					m_mutex.unlock();
					this->debug("Logger: Leaving thread {}", std::this_thread::get_id());
					m_mutex.lock();
					if (m_messages_count > 0)
						log_messages();
					break;
				}
			}
			m_thread_finished = true;
		};

	// init loggers
	for (auto i = 0; i < static_cast<std::uint64_t>(SIZE); ++i)
		m_loggers.push_back({ get_logger_name(static_cast<instance_index>(i)), &std::cout });

	m_thread = std::thread{ logger_thread };
	m_thread.detach();
	debug("Logger: OK");
}
void logger::on_detach(application* app)
{
	debug("Logger: OFF");
	m_should_close = true;
	if (m_thread_started && !m_thread_finished)
	{
		while (!m_thread_finished)
			_sleep(1);
	}
}
void logger::on_update(application* app)
{
}
}