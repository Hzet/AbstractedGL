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
		static constexpr std::uint64_t get_id() noexcept;
		static constexpr std::string_view get_name() noexcept;

	private:
		static std::uint64_t constexpr m_id;
	};

	template <class C>
	constexpr std::uint64_t type_id<C>::get_id() noexcept
	{
		return &m_id;
	}

	template <class C>
	constexpr std::string_view type_id<C>::get_name() noexcept
	{
		constexpr auto const str = std::string_view{ AGL_FUNC_NAME };
		constexpr auto const l_arrow = str.find_first_of('<');
		constexpr auto const r_arrow = str.find_last_of('>');

		return str.substr(l_arrow + 1, r_arrow - l_arrow - 1);
	}


}