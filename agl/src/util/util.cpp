#include "agl/util/util.hpp"
#include <vector>

namespace agl
{
namespace util
{
namespace ns
{
std::string memory_size(std::uint64_t bytes)
{
	char const* const units[] = {
		"B",
		"KB",
		"MB",
		"GB",
		"TB",
		"PB",
	};

	auto units_size = sizeof(units) / sizeof(const char*);

	for (auto i = 0; i < units_size; ++i)
	{
		if (bytes < 1024)
			return std::to_string(bytes) + units[i];

		bytes /= 1024;
	}
	return std::to_string(bytes) + units[units_size - 1];
}
}
}
}