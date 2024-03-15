#include "agl/util/async.hpp"
#include "agl/core/threads.hpp"

namespace agl
{
condition_variable::condition_variable()
	: m_id{ threads::invalid_id() }
{
}
bool condition_variable::is_valid() const
{
	return m_id != threads::invalid_id();
}
mutex::mutex()
	: m_id{ threads::invalid_id() }
{
}
bool mutex::is_valid() const
{
	return m_id != threads::invalid_id();
}
}