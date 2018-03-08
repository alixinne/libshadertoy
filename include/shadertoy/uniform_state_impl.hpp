#include "shadertoy/uniform_state_decl.hpp"

namespace shadertoy
{

// The viewport resolution (z is pixel aspect ratio, usually 1.0)
DECLARE_UNIFORM(glm::vec3, iResolution, "vec3");

// Current time in seconds
DECLARE_UNIFORM(GLfloat, iTime, "float");

// Time it takes to render a frame, in seconds
DECLARE_UNIFORM(GLfloat, iTimeDelta, "float");

// Current frame
DECLARE_UNIFORM(GLint, iFrame, "int");

// Number of frames rendered per second
DECLARE_UNIFORM(GLfloat, iFrameRate, "float");

// Time for channel (if video or sound), in seconds
// Currently not used because video and sound is not supported
DECLARE_UNIFORM_N(GLfloat, iChannelTime, 4, "float");

// Input texture resolution for each channel
DECLARE_UNIFORM_N(glm::vec3, iChannelResolution, 4, "vec3");

// xy = current pixel coords (if LMB is down). zw = click pixel
DECLARE_UNIFORM(glm::vec4, iMouse, "vec4");

// Sampler for input texture 0
DECLARE_UNIFORM(GLint, iChannel0, "sampler2D");

// Sampler for input texture 1
DECLARE_UNIFORM(GLint, iChannel1, "sampler2D");

// Sampler for input texture 2
DECLARE_UNIFORM(GLint, iChannel2, "sampler2D");

// Sampler for input texture 3
DECLARE_UNIFORM(GLint, iChannel3, "sampler2D");

// Year, month, day, time in seconds in .xyzw
DECLARE_UNIFORM(glm::vec4, iDate, "vec4");

// The sound sample rate
// Currently not used because sound is not supported
DECLARE_UNIFORM(GLfloat, iSampleRate, "float");

}
