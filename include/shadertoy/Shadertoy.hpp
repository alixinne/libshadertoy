#include <boost/variant.hpp>
#include <map>
#include <sstream>
#include <array>

#include "shadertoy/pre.hpp"

#include "shadertoy/ShadertoyError.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "shadertoy/OpenGL/OpenGL.hpp"

#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/ContextConfig.hpp"
#include "shadertoy/UniformState.hpp"
#include "shadertoy/ToyBuffer.hpp"
#include "shadertoy/TextureEngine.hpp"
#include "shadertoy/RenderContext.hpp"
