#pragma once
#include <cstdint>
#include <string_view>
#include "agl/core/debug.hpp"

namespace agl
{
/**
 * @brief 
 * Represents the value returned by 'type_id::get_id'.
 */
class type_id_t
{
public:
	constexpr type_id_t()
		: m_id{ 0 }
	{
	}
	constexpr explicit type_id_t(std::uint64_t id)
		: m_id{ id }
	{
	}
	constexpr type_id_t(type_id_t&& other)
		: m_id{ other.m_id }
	{
	}
	constexpr type_id_t(type_id_t const& other)
		: m_id{ other.m_id }
	{
	}
	constexpr type_id_t& operator=(type_id_t&& other)
	{
		m_id = other.m_id;
		return *this;
	}
	constexpr type_id_t& operator=(type_id_t const& other)
	{
		m_id = other.m_id;
		return *this;
	}
	constexpr bool is_valid() const
	{
		return m_id != 0;
	}

	constexpr bool operator==(type_id_t other) const
	{
		return m_id == other.m_id;
	}

	constexpr bool operator!=(type_id_t other) const
	{
		return m_id != other.m_id;
	}

	constexpr operator std::uint64_t() const
	{
		return m_id;
	}

	constexpr std::uint64_t get_value() const
	{
		return m_id;
	}

private:
	std::uint64_t m_id;
};

namespace impl {
template <typename T>
class type_id
{
public:
	static constexpr type_id_t get_id()
	{
		return type_id_t{ (std::uint64_t)&m_id };
	}

	static constexpr std::string_view get_name()
	{
		constexpr auto const str = std::string_view{ AGL_FUNC_NAME };
		constexpr auto const search_offset = str.find("agl::type_id");
		constexpr auto const l_offset = str.find_first_of('<', search_offset) + 1;
		constexpr auto const r_offset = str.find_last_of('>');

		if constexpr (str.at(r_offset - 1) == ' ')
			return str.substr(l_offset, r_offset - l_offset - 1);
		return str.substr(l_offset, r_offset - l_offset);
	}

private:
	static constexpr std::uint64_t m_id{};
};
}

/**
 * @brief 
 * Provides a compile time method for users to identify types. Uses 'remove_cvref' on types.
 * @tparam T 
 */
template <typename T>
class type_id
{
public:
	using type = std::remove_reference_t<std::remove_const_t<T>>;

	static constexpr type_id_t get_id()
	{
		return impl::type_id<type>::get_id();
	}
	static constexpr std::string_view get_name()
	{
		return impl::type_id<type>::get_name();
	}
};

}
namespace std
{
/// <summary>
/// The specialized hasher structure.
/// </summary>
template <>
struct hash<agl::type_id_t>
{
	size_t operator()(agl::type_id_t uid) const
	{
		return hash<uint64_t>{}(uid.get_value());
	}
};
}