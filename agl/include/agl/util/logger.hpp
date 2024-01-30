#pragma once
#include <algorithm>
#include <iostream>
#include <tuple>
#include <mutex>
#include <thread>
#include "agl/core/application.hpp"
#include "agl/util/time.hpp"
#include "agl/util/deque.hpp"

namespace agl
{
class logger_instance
{
public:
	logger_instance() noexcept
		: m_stream{ nullptr }
	{
	}
	logger_instance(std::string const& name, std::ostream* stream) noexcept
		: m_name{ name }
		, m_stream{ stream }
		, m_messages{ 8 }
	{
	}
	logger_instance(logger_instance&& other) noexcept
		: m_name{ other.m_name }
		, m_stream{ other.m_stream }
	{
		other.m_stream = nullptr;
	}
	logger_instance& operator=(logger_instance&& other) noexcept
	{
		m_name = other.m_name;
		m_stream = other.m_stream;
		other.m_stream = nullptr;
		return *this;
	}
	logger_instance(logger_instance const&) noexcept = delete;
	logger_instance& operator=(logger_instance const&) noexcept = delete;
	std::string const& get_name() const noexcept
	{
		return m_name;
	}
	template <typename... TArgs>
	void log(std::string const& message, TArgs&&... args) noexcept
	{
		auto guard = std::lock_guard<std::mutex>{ m_mutex };
		m_messages.push_back(produce_message(message, std::forward<TArgs>(args)...);
	}

private:
	friend class logger;

private:
	template <typename... TArgs>
	std::string produce_message(std::string const& message, TArgs&&... args) noexcept
	{
		AGL_ASSERT(m_stream != nullptr, "invalid stream");

		auto result = message;
		auto dt = get_datetime();
		auto parsed = parse_arguments(message, std::forward_as_tuple(std::forward<TArgs>(args)...), std::make_index_sequence<sizeof... (TArgs)>{});
		auto found_l = std::uint64_t{};
		auto index = std::uint64_t{};
		auto current_index = std::uint64_t{};

		while (found_l != std::string::npos)
		{
			found_l = message.find_first_of("{");

			if (found_l != 0 && message[found_l - 1] == '\\')
				continue;

			auto found_r = message.find_first_of("}", found_l);
			AGL_ASSERT(found_r != std::string::npos, "mismatched {} tokens");

			if (found_r - found_l > 0)
				index = std::stoi(message.substr(found_l + 1, found_r - found_l - 1);
			else
					index = current_index++;

			result.replace(result.cbegin() + found_l, result.cbegin() + found_r + 1, parsed[index]);
		}

		(*m_stream) << to_string(dt) << "[" << m_name << "] " << result;
	}
	template <typename TTuple, std::uint64_t... TSequence> 
	vector<std::string> parse_arguments(std::string const& message, TTuple&& tuple, std::index_sequence<TSequence...>) noexcept
	{
		auto vec = vector<std::string>{};
		(vec.push_back(to_string(std::get<TSequence>(tuple))), ...);

		return vec;
	}
	template <typename T>
	static std::string to_string(T&& v) noexcept
	{
		auto ss = std::stringstream{};
		ss << v;
		return ss.str();
	}

private:
	deque<std::string> m_messages;
	std::string m_name;
	std::mutex m_mutex;
	std::ostream* m_stream;
};

class logger
	: public application_resource
{
public:
	logger() noexcept
		: application_resource{ type_id<logger>::get_id() }
	{
	}
	logger(logger&&) noexcept = default;
	logger(logger const&) noexcept = delete;
	logger& operator=(logger&&) noexcept = default;
	logger& operator=(logger const&) noexcept = delete;
	~logger() noexcept = default;
	template <typename... TArgs>
	void debug(std::string const& message, TArgs&&... args) noexcept
	{
		AGL_ASSERT(m_debug != nullptr, "output stream not initialized");
		m_debug.log(message, std::forward<TArgs>(args)...);
	}
	template <typename... TArgs>
	void error(std::string const& message, TArgs&&... args) noexcept
	{
		AGL_ASSERT(m_error != nullptr, "output stream not initialized");
		m_error.log(message, std::forward<TArgs>(args)...);
	}
	template <typename... TArgs>
	void info(std::string const& message, TArgs&&... args) noexcept
	{
		AGL_ASSERT(m_info != nullptr, "output stream not initialized");
		m_info.log(message, std::forward<TArgs>(args)...);
	}
	template <typename... TArgs>
	void trace(std::string const& message, TArgs&&... args) noexcept
	{
		AGL_ASSERT(m_trace != nullptr, "output stream not initialized");
		m_trace.log(message, std::forward<TArgs>(args)...);
	}
	template <typename... TArgs>
	void warning(std::string const& message, TArgs&&... args) noexcept
	{
		AGL_ASSERT(m_debug != nullptr, "output stream not initialized");
		m_warning.log(m_warning, std::forward<TArgs>(args)...);
	}

	virtual void on_attach(application* app) noexcept
	{
		// listener thread waiting for producer 'on_update' to take over messages from loggers.
	}

	virtual void on_detach(application* app) noexcept
	{

	}

	virtual void on_update(application* app) noexcept
	{
		// producer on main thread sending sorted array of messages from all loggers to consumer thread

	}

private:
	vector<std::string> get_messages() noexcept
	{
		auto messages = vector<std::string>{};
		auto retriever = [&](logger_instance& l)
			{
				messages.resize(messages.size() + l.m_messages.size());
				for (auto const& str : l.m_messages)
					messages.push_back(str);
			};

		retriever(m_debug);
		retriever(m_error);
		retriever(m_info);
		retriever(m_trace);
		retriever(m_warning);
		std::sort(messages.cbegin(), messages.cend());
		return messages;
	}

private:
	logger_instance m_debug;
	logger_instance m_error;
	logger_instance m_info;
	logger_instance m_trace;
	logger_instance m_warning;
};
}