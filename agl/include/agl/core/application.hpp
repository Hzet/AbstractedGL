#pragma once
#include <memory>
#include "agl/core/resources.hpp"

int main(int, char**);

namespace agl
{
class application;

extern std::unique_ptr<application> create_application();

class application
{
public: 
	struct properties
	{
		bool is_open;
	};

public:
	application() = default;
	application(application&&) = default;
	application(application const&) = delete;
	application& operator=(application&&) = default;
	application& operator=(application const&) = delete;

	properties const& get_properties() const;
	void close();
	virtual void init();

	resources& get_resources() noexcept;

private:
	friend int ::main(int, char**);

private:
	void run();
	
private:
	properties m_properties;
	resources m_resources;
};
}