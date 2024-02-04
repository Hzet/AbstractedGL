#pragma once
#include <chrono>
#include <string>
#include <ostream>

namespace agl
{
struct datetime
{
	std::uint64_t years;
	std::uint64_t days;
	std::uint64_t months;
	std::uint64_t hours;
	std::uint64_t minutes;
	std::uint64_t seconds;
	std::uint64_t milliseconds;
};

inline std::string to_string(datetime const& d) noexcept
{
	return std::to_string(d.days) + "/"
		+ std::to_string(d.months) + "/"
		+ std::to_string(d.years) + " "
		+ std::to_string(d.hours) + ":"
		+ std::to_string(d.minutes) + ":"
		+ std::to_string(d.seconds) + ":"
		+ std::to_string(d.milliseconds);
}

inline datetime get_datetime() noexcept
{
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	auto dt = datetime{};

	dt.years = static_cast<decltype(dt.years)>(time / (365ll * 24 * 60 * 60 * 1000 * 1000));
	time %= (365ll * 24 * 60 * 60 * 1000 * 1000);
	dt.months = static_cast<decltype(dt.months)>(time / (12ll * 24 * 60 * 60 * 1000 * 1000));
	time %= (12ll * 24 * 60 * 60 * 1000 * 1000);
	dt.days = static_cast<decltype(dt.days)>(time / (24ll * 60 * 60 * 1000 * 1000));
	time %= (24ll * 60 * 60 * 1000 * 1000);
	dt.hours = static_cast<decltype(dt.hours)>(time / (60ll * 60 * 1000 * 1000));
	time %= (60ll * 60 * 1000 * 1000);
	dt.minutes = static_cast<decltype(dt.minutes)>(time / (60 * 1000 * 1000));
	time %= (60 * 1000 * 1000);
	dt.seconds = static_cast<decltype(dt.seconds)>(time / (1000 * 1000));
	time %= (1000 * 1000);
	dt.milliseconds = static_cast<decltype(dt.milliseconds)>(time / 1000);

	return dt;
}

inline std::ostream& operator<<(std::ostream& stream, datetime const& dt) noexcept
{
	return stream << dt.days << "/"
		<< dt.months << "/"
		<< dt.years << " "
		<< dt.hours << ":"
		<< dt.minutes << ":"
		<< dt.seconds << ":"
		<< dt.milliseconds;
}
}