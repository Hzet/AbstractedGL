#pragma once
#include <cstdint>
#include <string_view>
#include "agl/core/debug.hpp"

namespace agl
{
	template <class C>
	class type_id
	{
	public:
		static constexpr std::uint64_t const*  const get_id() noexcept
		{
			return &m_id;
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
		static std::uint64_t constexpr m_id{};
	};
}