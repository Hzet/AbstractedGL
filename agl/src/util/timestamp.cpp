#include "agl/util/timestamp.hpp"

namespace agl
{
timestamp timestamp::from_nano(std::uint64_t nano)
{
	return timestamp{ nano };
}
timestamp timestamp::from_micro(std::uint64_t micro)
{
	return timestamp{ micro * 1000 };
}
timestamp timestamp::from_milli(std::uint64_t milli)
{
	return timestamp{ milli * 1000 * 1000 };
}
timestamp timestamp::from_seconds(std::uint64_t seconds)
{
	return timestamp{ seconds * 1000 * 1000 * 1000 };
}
timestamp timestamp::from_minutes(std::uint64_t minutes)
{
	return timestamp{ minutes * 1000 * 1000 * 1000 * 60 };
}
timestamp timestamp::from_hours(std::uint64_t hours)
{
	return timestamp{ hours * 1000 * 1000 * 1000 * 60 * 60 };
}
timestamp timestamp::from_days(std::uint64_t days)
{
	return timestamp{ days * 1000 * 1000 * 1000 * 60 * 60 * 24 };
}
timestamp timestamp::from_months(std::uint64_t months)
{
	return timestamp{ months * 1000 * 1000 * 1000 * 60 * 60 * 24 * 30 };
}
timestamp timestamp::from_years(std::uint64_t years)
{
	return timestamp{ years * 1000 * 1000 * 1000 * 60 * 60 * 24 * 30 * 365 };
}
timestamp::timestamp(std::uint64_t nano)
	: m_nano{ nano }
{
}
std::uint64_t timestamp::get_nano() const
{
	return m_nano;
}
std::uint64_t timestamp::get_micro() const
{
	return m_nano / 1000;
}
std::uint64_t timestamp::get_milli() const
{
	return m_nano / (1000 * 1000);
}
std::uint64_t timestamp::get_seconds() const
{
	return m_nano / (1000 * 1000 * 1000);
}
std::uint64_t timestamp::get_minutes() const
{
	return m_nano / (1000 * 1000 * 1000 * 60);
}
std::uint64_t timestamp::get_hours() const
{
	return m_nano / (1000 * 1000 * 1000 * 60 * 60);
}
std::uint64_t timestamp::get_days() const
{
	return m_nano / (1000 * 1000 * 1000 * 60 * 60 * 24);
}
std::uint64_t timestamp::get_months() const
{
	return m_nano / (1000 * 1000 * 1000 * 60 * 60 * 24 * 30);
}
std::uint64_t timestamp::get_years() const
{
	return m_nano / (1000 * 1000 * 1000 * 60 * 60 * 24 * 30 * 365);
}
timestamp& timestamp::operator=(timestamp other)
{
	m_nano = other.m_nano;
	return *this;
}
timestamp& timestamp::operator=(std::uint64_t nano)
{
	m_nano = nano;
	return *this;

}
timestamp& timestamp::operator+=(timestamp rhs)
{
	m_nano += rhs.m_nano;
	return *this;
}
timestamp& timestamp::operator-=(timestamp rhs)
{
	m_nano -= rhs.m_nano;
	return *this;

}
timestamp& timestamp::operator/=(std::uint64_t factor)
{
	m_nano /= factor;
	return *this;

}
timestamp& timestamp::operator*=(std::uint64_t factor)
{
	m_nano *= factor;
	return *this;
}
timestamp operator+(timestamp lhs, timestamp rhs)
{
	return timestamp{ lhs.get_nano() + rhs.get_nano() };
}
timestamp operator-(timestamp lhs, timestamp rhs)
{
	return timestamp{ lhs.get_nano() - rhs.get_nano() };
}
timestamp operator/(timestamp lhs, std::uint64_t factor)
{
	return timestamp{ lhs.get_nano() / factor };
}
timestamp operator*(timestamp lhs, std::uint64_t factor)
{
	return timestamp{ lhs.get_nano() * factor };
}
bool operator==(timestamp lhs, std::uint64_t nano)
{
	return lhs.get_nano() == nano;
}
bool operator!=(timestamp lhs, std::uint64_t nano)
{
	return lhs.get_nano() != nano;
}
bool operator<=(timestamp lhs, std::uint64_t nano)
{
	return lhs.get_nano() <= nano;
}
bool operator<(timestamp lhs, std::uint64_t nano)
{
	return lhs.get_nano() < nano;
}
bool operator>(timestamp lhs, std::uint64_t nano)
{
	return lhs.get_nano() > nano;
}
bool operator>=(timestamp lhs, std::uint64_t nano)
{
	return lhs.get_nano() >= nano;
}
bool operator==(timestamp lhs, timestamp rhs)
{
	return lhs.get_nano() == rhs.get_nano();
}
bool operator!=(timestamp lhs, timestamp rhs)
{
	return lhs.get_nano() != rhs.get_nano();
}
bool operator<=(timestamp lhs, timestamp rhs)
{
	return lhs.get_nano() <= rhs.get_nano();
}
bool operator<(timestamp lhs, timestamp rhs)
{
	return lhs.get_nano() < rhs.get_nano();
}
bool operator>(timestamp lhs, timestamp rhs)
{
	return lhs.get_nano() > rhs.get_nano();
}
bool operator>=(timestamp lhs, timestamp rhs)
{
	return lhs.get_nano() >= rhs.get_nano();
}
}