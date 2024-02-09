#pragma once
#include "agl/util/memory/deque.hpp"
#include "agl/util/memory/set.hpp"
#include "agl/util/typeid.hpp"

namespace agl
{
namespace ecs
{
// make this CRTP
class component_storage_base
{
public:
	component_storage_base() noexcept = default;
	component_storage_base(component_storage_base&&) noexcept = default;
	component_storage_base& operator=(component_storage_base&&) noexcept = default;
	virtual ~component_storage_base() noexcept = default;
	virtual void dummy() noexcept = 0;
};

template <typename T>
class component_storage
	: public component_storage_base
{
public:
	virtual void dummy() noexcept override 
	{
	}

	mem::deque<T> storage;
};
}
}