#include "agl/util/thread.hpp"
#include "agl/core/threads.hpp"

namespace agl
{
thread::thread()
	: m_is_running{ false }
	//, m_my_cond_var{ nullptr }
	//, m_my_mutex{ nullptr }
	, m_internal_id{ threads::invalid_id() }
{
}
bool thread::is_valid() const
{
	return m_internal_id != threads::invalid_id();//&& m_my_cond_var != nullptr && m_my_mutex != nullptr;
}
}