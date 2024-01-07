#pragma once
#include <cstddef>

#include "agl/util/memory/pool.hpp"
#include "agl/util/memory/containers.hpp"

namespace agl
{
namespace ecs
{
class entity
{
public:
	template <typename T>
	bool has_component() const noexcept;

	bool has_component(std::uint64_t id) const noexcept;

	std::vector<std::uint64_t> get_component_ids() const noexcept;

	template <typename T>
	agl::mem::vector<std::byte> const& get_components() const noexcept;

private:
	agl::mem::unordered_map<std::uint64_t, agl::mem::vector<std::byte*>> m_components;
};

class component_array
{
public:
	template <typename T>
	T* add(T&& value);
	std::uint64_t size() const noexcept;

private:
	agl::mem::vector<std::byte> m_ptrs;
};

class organizer
{
public:
	organizer() noexcept = default;
	~organizer() noexcept = default;



private:
	agl::mem::vector<
	agl::mem::vector<entity> m_entities;
	agl::mem::vector<int> m_vec;
};
}
}