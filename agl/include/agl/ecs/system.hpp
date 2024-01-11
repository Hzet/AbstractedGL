#pragma once
#include <cstdint>
#include <string>
#include "agl/util/memory/containers.hpp"

namespace agl
{
namespace ecs
{
	class organizer;
	
	struct system
	{
		using function = void(*)(organizer*, float);

		function fun;
		std::string name;
		std::uint64_t stage;
	};
}
}