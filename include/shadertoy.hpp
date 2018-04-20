#include <array>
#include <memory>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/filesystem/path.hpp>

#include "shadertoy/pre.hpp"

#include "shadertoy/gl.hpp"

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

#include "shadertoy/uniform_state.hpp"

#include "shadertoy/compiler/basic_part.hpp"
#include "shadertoy/compiler/define_part.hpp"
#include "shadertoy/compiler/shader_template.hpp"
#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/members/basic_member.hpp"
#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/members/screen_member.hpp"

#include "shadertoy/render_context.hpp"
#include "shadertoy/swap_chain.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"
#include "shadertoy/buffers/gl_buffer.hpp"
#include "shadertoy/buffers/toy_buffer.hpp"

#include "shadertoy/utils.hpp"
