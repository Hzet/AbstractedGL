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
	component_storage() {}
	component_storage(typename mem::deque<T>::allocator_type const& allocator)
		: m_storage{ allocator }
	{
	}
	component_storage(typename mem::deque<T>::allocator_type const& allocator, std::uint64_t block_size)
		: m_storage{ allocator, block_size }
	{
	}
	component_storage(component_storage&& other)
		: m_storage{ std::move(other.m_storage) }
	{
	}
	component_storage& operator=(component_storage&& other)
	{
		m_storage = std::move(other.m_storage);
		return *this;
	}
	~component_storage() {}
	template <typename... TArgs> 
	T* push_component(TArgs&&... args)
	{
		return &(*m_storage.emplace_back(std::forward<TArgs>(args)...));
	}
	virtual void pop_component(std::byte* ptr) override
	{
		m_storage.erase(reinterpret_cast<T*>(ptr));
	}

	virtual std::uint64_t size() const override
	{
		return m_storage.size();
	}

private:
	mem::deque<T> m_storage;
};
}
}