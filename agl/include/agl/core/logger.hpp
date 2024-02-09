#pragma once
#include <iostream>
#include <tuple>
#include <sstream>
#include "date/date.h"
#include "agl/core/application.hpp"
#include "agl/util/time.hpp"
#include "agl/util/async.hpp"
#include "agl/util/deque.hpp"

namespace agl
{
class thread;

class logger
	: public resource<logger>
{
public:
	logger() noexcept;
	logger(logger&& other) noexcept;
	logger(logger const&) noexcept = delete;
	logger& operator=(logger const&) noexcept = delete;
	~logger() noexcept = default;

	template <typename... TArgs>
	void debug(std::string const& message, TArgs&&... args) noexcept;

	template <typename... TArgs>
	void error(std::string const& message, TArgs&&... args) noexcept;

	template <typename... TArgs>
	void info(std::string const& message, TArgs&&... args) noexcept;

	template <typename... TArgs>
	void trace(std::string const& message, TArgs&&... args) noexcept;

	template <typename... TArgs>
	void warning(std::string const& message, TArgs&&... args) noexcept;

private:
	enum instance_index
	{
		DEBUG,
		ERROR,
		INFO,
		TRACE,
		WARNING,
		SIZE
	};

	struct instance
	{
		std::string m_name;
		std::ostream* m_stream;
	};

	struct message
	{
		instance_index destination;
		std::string message;
	};

private:
	static std::string get_date() noexcept;
	static const char* get_logger_name(instance_index index) noexcept;
	bool is_active() const noexcept;
	virtual void on_attach(application* app) noexcept override;
	virtual void on_detach(application* app) noexcept override;
	virtual void on_update(application* app) noexcept override;

	template <typename T>
	static std::string to_string(T&& v) noexcept;

	template <typename... TArgs> 
	void log(instance_index index, std::string const& message, TArgs&&... args) noexcept;

	template <typename... TArgs>
	static std::string produce_message(instance_index index, std::string message, TArgs&&... args) noexcept;

	template <typename TTuple, std::uint64_t... TSequence>
	static vector<std::string> parse_arguments(TTuple&& tuple, std::index_sequence<TSequence...>) noexcept;

private:
	std::atomic<std::uint64_t> m_messages_count;
	vector<message> m_messages;
	condition_variable* m_cond_var;
	vector<instance> m_loggers;
	thread* m_thread;
	mutex* m_mutex;

};

template <typename... TArgs>
void logger::debug(std::string const& message, TArgs&&... args) noexcept
{
	log(DEBUG, message, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void logger::error(std::string const& message, TArgs&&... args) noexcept
{
	log(ERROR, message, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void logger::info(std::string const& message, TArgs&&... args) noexcept
{
	log(INFO, message, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void logger::trace(std::string const& message, TArgs&&... args) noexcept
{
	log(TRACE, message, std::forward<TArgs>(args)...);
}
template <typename... TArgs>
void logger::warning(std::string const& message, TArgs&&... args) noexcept
{
	log(WARNING, message, std::forward<TArgs>(args)...);
}

template <typename T>
std::string logger::to_string(T&& v) noexcept
{
	auto ss = std::stringstream{};
	ss << v;
	return ss.str();
}

template <typename... TArgs>
void logger::log(instance_index index, std::string const& message, TArgs&&... args) noexcept
{
	AGL_ASSERT(m_mutex != nullptr, "operation on uninitialized object");
	AGL_ASSERT(m_cond_var != nullptr, "operation on uninitialized object");
	AGL_ASSERT(is_active(), "logger is inactive");

	auto const msg = get_date() + " " + produce_message(index, message, std::forward<TArgs>(args)...);
	auto time = std::chrono::system_clock::now();
	{
		std::lock_guard<std::mutex> lock{ *m_mutex };
		m_messages.push_back({ index, msg });
	}
	++m_messages_count;
	m_cond_var->notify_one();
}
template <typename... TArgs>
std::string logger::produce_message(instance_index index, std::string message, TArgs&&... args) noexcept
{
	auto const parsed = parse_arguments(std::forward_as_tuple(std::forward<TArgs>(args)...), std::make_index_sequence<sizeof... (TArgs)>{});
	auto found_l = std::uint64_t{};
	auto arg_index = std::uint64_t{};
	auto current_index = std::uint64_t{};

	for (found_l = message.find_first_of("{"); found_l != std::string::npos; found_l = message.find_first_of("{", found_l + 1))
	{
		if (found_l > 0 && message[found_l - 1] == '\\')
			continue;

		auto found_r = message.find_first_of("}", found_l);
		AGL_ASSERT(found_r != std::string::npos, "mismatched {} tokens");

		auto const token_size = found_r - found_l + 1;
		if (token_size > 2)
			arg_index = std::stoi(message.substr(found_l + 1, found_r - found_l - 1));
		else
			arg_index = current_index++;

		AGL_ASSERT(arg_index < parsed.size(), "too few arguments provided");
		message.replace(message.cbegin() + found_l, message.cbegin() + found_r + 1, parsed[arg_index]);

		found_l += parsed[arg_index].size() - token_size;
	}
	message.insert(0, "] ");
	message.insert(0, get_logger_name(index));
	message.insert(0, "[");
	return message;
}
template <typename TTuple, std::uint64_t... TSequence>
vector<std::string> logger::parse_arguments(TTuple&& tuple, std::index_sequence<TSequence...>) noexcept
{
	auto vec = vector<std::string>{};
	(vec.push_back(to_string(std::get<TSequence>(tuple))), ...);

	return vec;
}
}