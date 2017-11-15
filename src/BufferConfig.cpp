#include "stdafx.hpp"
#include "shadertoy/BufferConfig.hpp"

using namespace std;

namespace fs = boost::filesystem;

using namespace shadertoy;

bool InputConfig::enabled() const
{
	return !id.empty() && !type.empty();
}

InputConfig::InputConfig()
	: id(),
	  type(),
	  source(),
	  wrap(GL_REPEAT),
	  minFilter(GL_LINEAR),
	  magFilter(GL_LINEAR)
{
}
