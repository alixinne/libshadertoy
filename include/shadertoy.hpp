#include "shadertoy/pre.hpp"

#include "shadertoy/gl.hpp"

#include "shadertoy/shadertoy_error.hpp"
#include "shadertoy/size.hpp"
#include "shadertoy/member_swap_policy.hpp"

#include "shadertoy/buffers/basic_buffer.hpp"
#include "shadertoy/buffers/graphics_buffer.hpp"
#include "shadertoy/buffers/gl_buffer.hpp"

#include "shadertoy/compiler/basic_part.hpp"
#include "shadertoy/compiler/define_part.hpp"
#include "shadertoy/compiler/file_part.hpp"
#include "shadertoy/compiler/input_part.hpp"
#include "shadertoy/compiler/shader_template.hpp"
#include "shadertoy/compiler/template_error.hpp"
#include "shadertoy/compiler/template_part.hpp"

#include "shadertoy/compiler/program_template.hpp"

#include "shadertoy/geometry/basic_geometry.hpp"
#include "shadertoy/geometry/screen_quad.hpp"

#include "shadertoy/inputs/basic_input.hpp"
#include "shadertoy/inputs/buffer_input.hpp"
#include "shadertoy/inputs/checker_input.hpp"
#include "shadertoy/inputs/error_input.hpp"
#include "shadertoy/inputs/file_input.hpp"
#include "shadertoy/inputs/image_input.hpp"
#include "shadertoy/inputs/noise_input.hpp"
#include "shadertoy/inputs/openimageio_input.hpp"
#include "shadertoy/inputs/texture_input.hpp"

#include "shadertoy/draw_state.hpp"

#include "shadertoy/io_resource.hpp"

#include "shadertoy/members/basic_member.hpp"
#include "shadertoy/members/buffer_member.hpp"
#include "shadertoy/members/compute_member.hpp"
#include "shadertoy/members/screen_member.hpp"

#include "shadertoy/buffers/program_buffer.hpp"
#include "shadertoy/buffers/toy_buffer.hpp"
#include "shadertoy/buffers/geometry_buffer.hpp"

#include "shadertoy/buffers/compute_buffer.hpp"

#include "shadertoy/program_interface.hpp"

#include "shadertoy/render_context.hpp"
#include "shadertoy/shader_compiler.hpp"
#include "shadertoy/swap_chain.hpp"

#include "shadertoy/utils.hpp"

