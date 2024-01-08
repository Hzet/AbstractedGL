#pragma once
#include "agl/ecs/storage.hpp"
#include "agl/ecs/entity.hpp"

namespace agl
{
namespace ecs
{
class organizer
{
public:
	organizer() noexcept = default;
	~organizer() noexcept = default;



private:
	impl::storage<impl::entity_data> m_entities;
};
}
}