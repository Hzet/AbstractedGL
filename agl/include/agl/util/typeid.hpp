#pragma once
#include <cstdint>
#include <string_view>
#include "agl/core/debug.hpp"

namespace agl
{
	class type_id_t
	{
	public:
		constexpr type_id_t() noexcept
			: m_id{ 0 }
		{
		}
		constexpr explicit type_id_t(std::uint64_t id) noexcept
			: m_id{ id }
		{
		}

		constexpr type_id_t(type_id_t const& other) noexcept
			: m_id{ other.m_id }
		{
		}
		
		constexpr bool is_valid() const noexcept
		{
			return m_id != 0;
		}

		constexpr bool operator==(type_id_t other) const noexcept
		{
			return m_id == other.m_id;
		}

		constexpr bool operator!=(type_id_t other) const noexcept
		{
			return m_id == other.m_id;
		}

		constexpr operator std::uint64_t() const noexcept
		{
			return m_id;
		}

		constexpr std::uint64_t get_value() const noexcept
		{
			return m_id;
		}

	private:
		std::uint64_t m_id;
	};

	template <typename T>
	class type_id
	{
	public:
		static constexpr type_id_t get_id() noexcept
		{
			return type_id_t{ (std::uint64_t)&m_id };
		}

		static constexpr std::string_view get_name() noexcept
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

namespace std
{
/// <summary>
/// The specialized hasher structure.
/// </summary>
template <>
struct hash<agl::type_id_t>
{
	size_t operator()(agl::type_id_t uid) const noexcept
	{
		return hash<uint64_t>{}(uid.get_value());
	}
};
}