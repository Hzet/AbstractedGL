#pragma once
#include <cstdint>

namespace agl
{
class timestamp final
{
public:
	static timestamp from_nano(std::uint64_t nano);
	static timestamp from_micro(std::uint64_t micro);
	static timestamp from_milli(std::uint64_t milli);
	static timestamp from_seconds(std::uint64_t seconds);
	static timestamp from_minutes(std::uint64_t minutes);
	static timestamp from_hours(std::uint64_t hours);
	static timestamp from_days(std::uint64_t days);
	static timestamp from_months(std::uint64_t months);
	static timestamp from_years(std::uint64_t years);

public:
	              timestamp(std::uint64_t nano = 0);
	std::uint64_t get_nano() const;
	std::uint64_t get_micro() const;
	std::uint64_t get_milli() const;
	std::uint64_t get_seconds() const;
	std::uint64_t get_minutes() const;
	std::uint64_t get_hours() const;
	std::uint64_t get_days() const;
	std::uint64_t get_months() const;
	std::uint64_t get_years() const;
	timestamp&    operator=(timestamp other);
	timestamp&    operator=(std::uint64_t nano);
	timestamp&    operator+=(timestamp rhs);
	timestamp&    operator-=(timestamp rhs);
	timestamp&    operator/=(std::uint64_t factor);
	timestamp&    operator*=(std::uint64_t factor);

private:
	std::uint64_t m_nano;
};

timestamp operator+(timestamp lhs, timestamp rhs);
timestamp operator-(timestamp lhs, timestamp rhs);
timestamp operator/(timestamp lhs, std::uint64_t factor);
timestamp operator*(timestamp lhs, std::uint64_t factor);
bool      operator==(timestamp lhs, std::uint64_t nano);
bool      operator!=(timestamp lhs, std::uint64_t nano);
bool      operator<=(timestamp lhs, std::uint64_t nano);
bool      operator<(timestamp lhs, std::uint64_t nano);
bool      operator>(timestamp lhs, std::uint64_t nano);
bool      operator>=(timestamp lhs, std::uint64_t nano);
bool      operator==(timestamp lhs, timestamp rhs);
bool      operator!=(timestamp lhs, timestamp rhs);
bool      operator<=(timestamp lhs, timestamp rhs);
bool      operator<(timestamp lhs, timestamp rhs);
bool      operator>(timestamp lhs, timestamp rhs);
bool      operator>=(timestamp lhs, timestamp rhs);
}