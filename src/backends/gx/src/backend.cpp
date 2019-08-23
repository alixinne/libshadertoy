#include "shadertoy/backends/gx/backend.hpp"

using namespace shadertoy::backends::gx;

backend::~backend() {}

namespace shadertoy
{
namespace backends
{
shadertoy_EXPORT thread_local std::unique_ptr<gx::backend> current = {};
}
} // namespace shadertoy
