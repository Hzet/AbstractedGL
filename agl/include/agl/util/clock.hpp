#pragma once
#include <cstdint>
#include <chrono>
#include "agl/util/timestamp.hpp"

namespace agl
{
class clock
{
public:
	static timestamp get_current_time();

public:
	timestamp        get_elapsed() const;
	void             reset();
	void             start();
	void             stop();

private:
	timestamp m_start_time;
	timestamp m_stopped_time;
};
}