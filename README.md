# libshadertoy

*libshadertoy* is a C++ library for rendering ShaderToy programs on an OpenGL
desktop. This repository also contains the needed files to package this library
as native Debian packages.

## Dependencies (local build)

The following tools and libraries are required:

* OpenGL 4.x
* C++14-enabled compiler (GCC 5.x or clang3.8+)
* [CMake](https://launchpad.net/ubuntu/xenial/+source/cmake)
* [Boost 1.58+](https://launchpad.net/ubuntu/xenial/+package/libboost-all-dev)
* [SOIL](https://launchpad.net/ubuntu/xenial/+package/libsoil-dev)
* [GLEW](https://launchpad.net/ubuntu/xenial/+source/glew)
* [oglplus](https://oglplus.org/). Automatically downloaded from GitHub if
needed, but an installed version will be used if available. Requires GLEW and
GLFW3 to be installed.

## Usage

This library only uses an existing OpenGL context that must be created and made
current before trying to render frames from a ShaderToy program. This can be
achieved using GLFW3 (recommended), GLUT or any other context creation library.
Extension loading is done using GLEW, so it should be used in the parent project
too.

See the `examples/` folder on how to use this library.

Note that only `iResolution`, `iTimeDelta`, `iFrameRate`, `iChannel0`,
`iChannel1`, `iChannel2` and `iChannel3` are automatically maintained by the
library render context. The host is responsible for updating other uniforms,
such as `iFrame`, `iGlobalTime` and such.

## Dependencies (package build)

In order to build the Debian packages for this library, the easiest way is to
use the provided `build.sh` script. It uses Docker to create a suitable build
environment, mounting the current source tree as a volume in order to fetch
the build artifacts. It is used as follows:

```bash
./build.sh -d [debian]
```

Note that your user account should be able to use the Docker command (ie. being
part of the *docker* system group). The first steps include creating the base
images with all the build dependencies, which can be a lengthy process.
Subsequent runs reuse the base image and are much faster.

In order to test the build packages, you can use the `autopkgtest` package. Note
that at this time, only the chroot method has been working. Here are the steps
to build a working chroot for autopkgtest.

```bash
# Required packages
sudo apt-get install -y binutils debootstrap

# Create the chroot *OUTSIDE* the build tree
mkdir ../../chroot

# Bootstrap Debian
sudo debootstrap --arch amd64 stretch ../../chroot

# Mount partitions from host system
sudo mount --rbind /dev ../../chroot/dev
sudo mount --bind /proc ../../chroot/proc
sudo mount --bind /sys ../../chroot/sys

# Mount the X11 socket
sudo mkdir -p  ../../chroot/tmp/.X11-unix
sudo mount --bind /tmp/.X11-unix ../../chroot/tmp/.X11-unix

# Switch into chroot
sudo chroot ../../chroot

# You need to install the same GL driver as the host, here for Nvidia
#  Enable non-free
sed -i 's/main$/main contrib non-free/' /etc/apt/sources.list
#  Install driver
apt-get update
apt-get install -y nvidia-driver

# You can also install the build dependencies beforehand
apt-get install -y build-essential libboost-all-dev libsoil-dev libglew-dev \
	libglfw3-dev cmake git ca-certificates

# Leave the chroot
exit
```

You can then run the tests using autopkgtest:

```bash
# Authorize local connections to X11
xhost +local:

# Run tests
sudo autopkgtest ../*.deb -- chroot ../../chroot
```
