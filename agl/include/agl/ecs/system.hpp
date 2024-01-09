#pragma once
#include <cstdint>

namespace agl
{
namespace ecs
{
	class organizer;
	
	struct system
	{
		using function = void()(organizer*, float);
	};
}
}