#pragma once
#include "agl/core/application.hpp"
#include "agl/core/event.hpp"
#include "agl/dictionary.hpp"
#include "agl/util/async.hpp"

namespace agl
{
namespace glfw
{
// GLFW 
class api final
	: public resource<api>
{
	virtual void on_attach(application* app) override;
	virtual void on_detach(application* app) override;
	virtual void on_update(application* app) override;
};
}
}
