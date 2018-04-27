# libshadertoy

[![Build Status](https://ci.inria.fr/libshadertoy/buildStatus/icon?job=libshadertoy)](https://ci.inria.fr/libshadertoy/job/libshadertoy/)

*libshadertoy* is a C++ library for rendering multipass programs using OpenGL
desktop. Its defaults allow running programs written for the
[shadertoy](https://www.shadertoy.com) website, but it can be easily extended.
This library can be either compiled from source, or installed from the built
Debian packages.

## Introduction

* [Installing](docs/INSTALL.md)
* [Testing](docs/TESTING.md)
* [Using the examples](docs/EXAMPLES.md)

## Usage

This library only uses an existing OpenGL context that must be created and made
current before trying to render frames from a program. This can be
achieved using GLFW3 (recommended), GLUT or any other context creation library.
Extension loading is done using libepoxy, although this is transparent to the
user of the library.

See the `examples/` folder on how to use this library.

Note that only `iResolution`, `iTimeDelta`, `iFrameRate`, `iChannel0`,
`iChannel1`, `iChannel2` and `iChannel3` are automatically maintained by the
library render context. The host is responsible for updating other uniforms,
such as `iFrame`, `iTime` and such.

## Copyright

libshadertoy - Vincent Tavernier <vince.tavernier@gmail.com>

```
License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
