#pragma once
#include <string>

namespace agl
{
class shader
{
public:
	virtual void load_from_file(std::string const& filepath) = 0;

private:
	std::string m_filepath;
};
}