# libshadertoy

*libshadertoy* is a C++ library for rendering ShaderToy programs on an OpenGL
desktop.

## Dependencies

The following tools and libraries are required:

* OpenGL 4.x
* C++14-enabled compiler (GCC 5.x or clang3.8+)
* [CMake](https://launchpad.net/ubuntu/xenial/+source/cmake)
* [Boost 1.58+](https://launchpad.net/ubuntu/xenial/+package/libboost-all-dev)
* [SOIL](https://launchpad.net/ubuntu/xenial/+package/libsoil-dev)
* [GLEW](https://launchpad.net/ubuntu/xenial/+source/glew)
* [oglplus](https://oglplus.org/). Automatically downloaded from github if
needed, but an installed version will be used if available. Requires GLEW and
GLFW3 to be installed.

## Usage

Note that this library only uses an existing OpenGL context that must be created
and made current before trying to render frames from a ShaderToy program. This
can be achieved using GLFW3 (recommended), GLUT or any other context creation
library. Extension loading is done using GLEW, so it should be used in the
parent project too.

Note that only `iResolution`, `iTimeDelta`, `iFrameRate`, `iChannel0`,
`iChannel1`, `iChannel2` and `iChannel3` are automatically maintained by the
library render context. The host is resposnible for updating other uniforms,
such as `iFrame`, `iGlobalTime` and such.
