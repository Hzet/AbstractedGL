#pragma once
#include "agl/render/window.hpp"

namespace agl
{
namespace opengl
{
class window
	: public agl::window
{
public:
	using agl::window::window;
	virtual void create(glm::uvec2 resolution, std::string const& title) override;
	virtual void feature_disable(feature_type feature) override;
	virtual void feature_enable(feature_type feature) override;
	virtual bool feature_status(feature_type feature) override;
	virtual void hint_api_version(std::uint64_t major, std::uint64_t minor) override;
};
}
}