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
#include "shadertoy/buffers/basic_buffer.hpp"
#include "shadertoy/buffers/toy_buffer.hpp"
#include "shadertoy/context_config.hpp"
#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/buffer_input.hpp"
#include "shadertoy/inputs/checker_input.hpp"
#include "shadertoy/inputs/error_input.hpp"
#include "shadertoy/inputs/exr_input.hpp"
#include "shadertoy/inputs/file_input.hpp"
#include "shadertoy/inputs/image_input.hpp"
#include "shadertoy/inputs/jpeg_input.hpp"
#include "shadertoy/inputs/noise_input.hpp"
#include "shadertoy/inputs/soil_input.hpp"
#include "shadertoy/render_context.hpp"
#include "shadertoy/texture_engine.hpp"
#include "shadertoy/uniform_state.hpp"

#include "shadertoy/utils.hpp"
