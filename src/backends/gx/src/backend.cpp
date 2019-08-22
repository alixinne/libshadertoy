#include "shadertoy/backends/gx/backend.hpp"

using namespace shadertoy::backends::gx;

backend::~backend()
{
}

namespace shadertoy
{
namespace backends
{
	thread_local std::unique_ptr<gx::backend> current = {};
}
}
