#include "stdafx.hpp"
#include "shadertoy/BufferConfig.hpp"

using namespace std;
using namespace oglplus;

namespace fs = boost::filesystem;

using namespace shadertoy;

bool InputConfig::enabled() const
{
	return !id.empty() && (!source.empty() || (type == "noise" || type == "checker"));
}

InputConfig::InputConfig()
	: id(),
	  type(),
	  source(),
	  wrap(TextureWrap::Repeat),
	  minFilter(TextureMinFilter::Linear),
	  magFilter(TextureMagFilter::Linear)
{
}
