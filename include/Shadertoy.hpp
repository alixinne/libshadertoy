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

#include "shadertoy/ShadertoyError.hpp"

#include "shadertoy/gl.hpp"

#include "shadertoy/BufferConfig.hpp"
#include "shadertoy/ContextConfig.hpp"
#include "shadertoy/UniformState.hpp"
#include "shadertoy/ToyBuffer.hpp"
#include "shadertoy/TextureEngine.hpp"
#include "shadertoy/RenderContext.hpp"
