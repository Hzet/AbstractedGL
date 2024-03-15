#pragma once
#include "agl/memory/deque.hpp"
#include "agl/memory/set.hpp"
#include "agl/util/typeid.hpp"

namespace agl
{
namespace ecs
{
// make this CRTP
class component_storage_base
{
public:
	component_storage_base() = default;
	component_storage_base(component_storage_base&&) = default;
	component_storage_base& operator=(component_storage_base&&) = default;
	virtual ~component_storage_base() = default;
	virtual void pop_component(std::byte* ptr) = 0;
	virtual std::uint64_t size() const = 0;
};

template <typename T>
class component_storage
	: public component_storage_base
{
public:
	virtual void pop_component(std::byte* ptr) override
	{
		storage.erase(reinterpret_cast<T*>(ptr));
	}

	virtual std::uint64_t size() const override
	{
		return storage.size();
	}

public:
	mem::deque<T> storage;
};
}
}