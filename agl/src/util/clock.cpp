#include "agl/util/clock.hpp"

namespace agl
{
using ::std::chrono::steady_clock;
using ::std::chrono::time_point;

timestamp clock::get_current_time()
{
	return timestamp{ steady_clock::now().time_since_epoch().count() };
}
timestamp clock::get_elapsed() const
{
	return get_current_time() - m_start_time;
}
void clock::reset()
{
	m_start_time = 0;
	m_stopped_time = 0;
}
void clock::start()
{
	switch (m_start_time.get_nano())
	{
	case 0: m_start_time = get_current_time(); break;
	default: m_start_time += m_stopped_time; break;
	}
}
void clock::stop()
{
	if(m_stopped_time == 0)
		m_stopped_time = get_current_time();
}
}