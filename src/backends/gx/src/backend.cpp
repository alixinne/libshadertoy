#include "shadertoy/backends/gx/backend.hpp"

using namespace shadertoy::backends::gx;

backend::~backend() {}

namespace shadertoy
{
namespace backends
{
thread_local std::unique_ptr<gx::backend> current_backend = {};

shadertoy_EXPORT const std::unique_ptr<gx::backend> &current()
{
	return current_backend;
}

shadertoy_EXPORT std::unique_ptr<gx::backend> set_current(std::unique_ptr<gx::backend> new_backend)
{
	auto result = std::move(current_backend);
	current_backend = std::move(new_backend);
	return result;
}
}
} // namespace shadertoy
