# libshadertoy

[![Build Status](https://travis-ci.com/vtavernier/libshadertoy.svg?token=qW3fGvxWik6fwUsruShT&branch=master)](https://travis-ci.com/vtavernier/libshadertoy) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/395ff6e1a5ff42cc935229139ad0a5de)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=vtavernier/libshadertoy&amp;utm_campaign=Badge_Grade) [![master docs](https://img.shields.io/badge/docs-master-blue.svg)](https://vtavernier.github.io/libshadertoy/) [![develop docs](https://img.shields.io/badge/docs-develop-blue.svg)](https://vtavernier.github.io/libshadertoy/develop/) [![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

*libshadertoy* is a C++ library for rendering multipass programs using OpenGL
desktop. Its defaults allow running programs written for the
[shadertoy](https://www.shadertoy.com) website, but it can be easily extended.
This library can be either compiled from source, or installed from the built
Debian packages.

## Introduction

* [Installing](docs/INSTALL.md)
* [Testing](docs/TESTING.md)
* [Using the examples](docs/EXAMPLES.md)

## Documentation

Read the [online documentation](https://vtavernier.github.io/libshadertoy/).

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
