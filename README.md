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
* [libjpeg](https://launchpad.net/ubuntu/xenial/+package/libjpeg-dev)
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
such as `iFrame`, `iTime` and such.

## Building the examples

These are the instructions to build the *00-build* sample from scratch.

```bash
# Copy the sample to a working directory
cp -r /usr/share/shadertoy/examples/00-build shadertoy-example
cd shadertoy-example

# With libshadertoy-dev and libshadertoy0, install additional dependencies
sudo apt-get install libglfw3-dev cmake git g++ ca-certificates pkg-config

# Create a build directory and make the project
mkdir build
cd build
cmake ..
make -j9

# Run the example
./example00-build
```

Other examples may require more dependencies, see the associated README for more
details.

## Building the packages

To build the library on the development machine (needed for running tests) the
`build.sh` script should be used. See `debian/control` for the up-to-date list
of build dependencies.

The packages for Ubuntu Xenial and Debian Stretch can be built using
[sbuild](https://wiki.debian.org/sbuild) and a suitably setup
[schroot](https://wiki.debian.org/Schroot). Here is a reminder on how to setup
such an environment (Debian Stretch instructions, based on provided links).

```bash
# Debian Stretch amd64
sudo sbuild-createchroot --include=eatmydata,ccache,gnupg stretch /disc/schroot/stretch-amd64-sbuild http://deb.debian.org/debian

# Ubuntu Xenial amd64
sudo sbuild-createchroot --include=eatmydata,ccache,gnupg xenial /disc/schroot/xenial-amd64-sbuild http://archive.ubuntu.com/ubuntu/
# The Ubuntu schroot must be edited to add universe and multiverse repositories
sudo schroot -c xenial-amd64-sbuild # then edit /etc/apt/sources.list accordingly
```

In order to test the build packages, you can use the `autopkgtest` package. Note
that at this time, only the schroot method has been working. Here are the steps
to build a working schroot for autopkgtest, using the schroot created for
building the packages.

```bash
# Switch into chroot
sudo schroot -c stretch-amd64-sbuild

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
sudo autopkgtest ../*.deb -- schroot stretch-amd64-sbuild
```

## Copyright

libshadertoy - Vincent Tavernier <vince.tavernier@gmail.com>
