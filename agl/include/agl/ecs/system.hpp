#pragma once
#include <cstdint>
#include <string>

namespace agl
{
class application;

namespace ecs
{
	struct stage
	{
		std::uint64_t id;
		std::string name;
	};

	class system
	{
	public:
		system() noexcept
		{
		}
		system(std::string const& name, ecs::stage stage) noexcept
			: m_name{ name }
			, m_stage{ stage }
		{
		}
		system(system&& other) noexcept
			: m_name{ other.m_name }
			, m_stage{ other.m_stage }
		{
		}
		system& operator=(system&& other) noexcept
		{
			m_name = m_name;
			m_stage = m_stage;
			return *this;
		}
		system(system const&) noexcept = delete;
		system& operator=(system const&) noexcept = delete;
		virtual ~system() noexcept = default;
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