#include <array>
#include <map>
#include <memory>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/variant.hpp>

#include "shadertoy/pre.hpp"

#include "shadertoy/gl.hpp"

#include "shadertoy/buffer_config.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/uniform_state.hpp"
#include "shadertoy/buffers/basic_buffer.hpp"
#include "shadertoy/buffers/toy_buffer.hpp"
#include "shadertoy/texture_engine.hpp"
#include "shadertoy/render_context.hpp"

#include "shadertoy/utils.hpp"
