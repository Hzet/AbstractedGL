#pragma once
#include <cstdint>
#include <string>
#include "agl/util/typeid.hpp"
#include "agl/memory/set.hpp"

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

	struct signal
	{
		std::uint64_t id;
		bool value;
	};

	struct signal_comp
	{
		bool operator()(signal const& lhs, std::uint64_t id) const noexcept
		{
			return lhs.id < id;
		}
		bool operator()(std::uint64_t id, signal const& rhs) const noexcept
		{
			return id < rhs.id;
		}
		bool operator()(signal const& lhs, signal const& rhs) const noexcept
		{
			return lhs.id < rhs.id;
		}
	};

	class system
	{
	public:
		system() noexcept;
		system(type_id_t id, std::string const& name, ecs::stage stage) noexcept;
		system(system&& other) noexcept;
		system& operator=(system&& other) noexcept;
		virtual ~system() noexcept = default;

		type_id_t id() const noexcept;
		std::string const& name() const noexcept;
		void name(std::string const& name);
		virtual void on_attach(application*) noexcept = 0;
		virtual void on_detach(application*) noexcept = 0;
		virtual void on_update(application*) noexcept = 0;
		stage stage() const noexcept;
		void stage(ecs::stage s) noexcept;
		bool read_signal(std::uint64_t id) noexcept;
		void set_signal(std::uint64_t id, bool value) noexcept;

	protected:
		void create_signal(std::uint64_t id, bool start_value);

	private:
		type_id_t m_id;
		std::string m_name;
		ecs::stage m_stage;
		mem::set<signal, signal_comp> m_signals;
	};
}
}