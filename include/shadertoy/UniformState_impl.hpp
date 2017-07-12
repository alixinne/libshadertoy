#include "shadertoy/UniformState_decl.hpp"

namespace shadertoy
{

DECLARE_UNIFORM(oglplus::Vec3f, iResolution);
DECLARE_UNIFORM(GLfloat, iGlobalTime);
DECLARE_UNIFORM(GLfloat, iTimeDelta);
DECLARE_UNIFORM(GLint, iFrame);
DECLARE_UNIFORM(GLfloat, iFrameRate);
DECLARE_UNIFORM(oglplus::Vec4f, iMouse);
DECLARE_UNIFORM(GLint, iChannel0);
DECLARE_UNIFORM(GLint, iChannel1);
DECLARE_UNIFORM(GLint, iChannel2);
DECLARE_UNIFORM(GLint, iChannel3);
DECLARE_UNIFORM(oglplus::Vec4f, iDate);
DECLARE_UNIFORM_N(oglplus::Vec3f, iChannelResolution, 4);

}
