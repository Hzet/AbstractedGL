#pragma once
#include <string>

namespace agl
{
namespace util
{
namespace ns // nice string, stuff returning stringified, ready to display informations in convenient fashion
{
std::string memory_size(std::uint64_t bytes);
}
}
}