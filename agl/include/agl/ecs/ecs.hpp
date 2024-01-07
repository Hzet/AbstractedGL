#pragma once
#include <cstddef>

#include "agl/util/memory/pool.hpp"
#include "agl/util/memory/containers.hpp"

namespace agl
{
namespace ecs
{
namespace impl
{
class component_tracker
{
public:
	

private:
	agl::mem::unordered_map<std::uint64_t, agl::mem::vector<std::byte*>> m_comp_ptrs;
};
}
class entity
{
public:
private:
	//agl::mem::vector<int> m_
};

class organizer
{
public:
	organizer() noexcept = default;
	~organizer() noexcept = default;

private:
	agl::mem::vector<int> m_vec;
};
}
}