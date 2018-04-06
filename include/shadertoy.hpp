#include <array>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/variant.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "shadertoy/pre.hpp"

#include "shadertoy/shadertoy_error.hpp"

#include "shadertoy/gl.hpp"

#include "shadertoy/buffer_config.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/uniform_state.hpp"
#include "shadertoy/toy_buffer.hpp"
#include "shadertoy/texture_engine.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/utils.hpp"
