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
		system() noexcept;
		system(std::string const& name, ecs::stage stage) noexcept;
		system(system&& other) noexcept;
		system& operator=(system&& other) noexcept;
		system(system const&) noexcept = delete;
		system& operator=(system const&) noexcept = delete;
		virtual ~system() noexcept = default;

		std::string const& name() const noexcept;
		void name(std::string const& name);
		virtual void on_attach(application*) noexcept = 0;
		virtual void on_detach(application*) noexcept = 0;
		virtual void on_update(application*) noexcept = 0;
		stage stage() const noexcept;
		void stage(ecs::stage s) noexcept;

	private:
		std::string m_name;
		ecs::stage m_stage;
	};
}
}