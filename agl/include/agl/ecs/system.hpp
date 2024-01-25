#pragma once
#include <cstdint>
#include <string>
#include "agl/util/memory/containers.hpp"

namespace agl
{
class application;

namespace ecs
{
	class organizer;
	
	struct stage
	{
		std::uint64_t id;
		std::string name;
	};

	class system
	{
	public:
		system(std::string const& name = "", ecs::stage stage = {})
			: m_name{ name }
			, m_stage{ stage }
		{
		}

		virtual void on_attach(application*) noexcept = 0;
		virtual void on_update(application*) noexcept = 0;
		virtual void on_detach(application*) noexcept = 0;

		std::string const& name() const noexcept
		{
			return m_name;
		}
		void name(std::string const& name)
		{
			m_name = name;
		}
		stage stage() const noexcept
		{
			return m_stage;
		}
		void stage(ecs::stage s) noexcept
		{
			m_stage = s;
		}

	private:
		std::string m_name;
		ecs::stage m_stage;
	};
}
}