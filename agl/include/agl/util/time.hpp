#pragma once
#include <chrono>
#include <string>

namespace agl
{
struct datetime
{
	std::uint32_t years;
	std::uint16_t days;
	std::uint8_t months;
	std::uint8_t hours;
	std::uint8_t minutes;
	std::uint8_t seconds;
	std::uint8_t milliseconds;
};

std::string to_string(datetime const& d) noexcept
{
	return std::to_string(d.days) + "/"
		+ std::to_string(d.months) + "/"
		+ std::to_string(d.years) + " "
		+ std::to_string(d.hours) + ":"
		+ std::to_string(d.minutes) + ":"
		+ std::to_string(d.seconds) + ":"
		+ std::to_string(d.milliseconds);
}

datetime get_datetime() noexcept
{
	auto time = std::chrono::system_clock::now().time_since_epoch().count();
	auto dt = datetime{};

	dt.years = time / (365l * 24 * 60 * 60 * 1000 * 1000);
	time %= (365l * 24 * 60 * 60 * 1000 * 1000);
	dt.months = time / (12l * 24 * 60 * 60 * 1000 * 1000);
	time %= (12l * 24 * 60 * 60 * 1000 * 1000);
	dt.days = time / (24l * 60 * 60 * 1000 * 1000);
	time %= (24l * 60 * 60 * 1000 * 1000);
	dt.hours = time / (60l * 60 * 1000 * 1000);
	time %= (60l * 60 * 1000 * 1000);
	dt.minutes = time / (60 * 1000 * 1000);
	time %= (60 * 1000 * 1000);
	dt.seconds = time / (1000 * 1000);
	time %= (1000 * 1000);
	dt.milliseconds = time / 1000;

	return dt;
}
}