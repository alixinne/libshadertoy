# Installing

There are several options for installing libshadertoy.

## Install the Debian/Ubuntu packages

A package repository is maintained for Debian Stretch (9) and Ubuntu Xenial
(16.04 LTS). It can be used to install a pre-built version of the library with
all its features:

```bash
# Get repository GPG key
sudo apt-key adv --recv-keys --keyserver pool.sks-keyservers.net --recv 0xF35C413E

# Add repository to apt config
#  Debian Stretch
sudo sh -c 'echo "deb https://dl.bintray.com/vtavernier/libshadertoy stretch main" >/etc/apt/sources.list.d/libshadertoy-apt.list'
#  Ubuntu Xenial (16.04)
sudo sh -c 'echo "deb https://dl.bintray.com/vtavernier/libshadertoy xenial main" >/etc/apt/sources.list.d/libshadertoy-apt.list'

# Update and install
sudo apt update
sudo apt install libshadertoy-dev
```

## Build from source

To build the library from source, first clone the repository as well as its
dependencies:

```bash
git clone --recursive https://gitlab.inria.fr/vtaverni/libshadertoy.git
```

In order to compile the project, the following tools and libraries are required:

* OpenGL 4.5
* C++14-enabled compiler (GCC 5 or Clang 3.4)
* [CMake 3.1+](https://launchpad.net/ubuntu/xenial/+source/cmake)
* [Boost 1.54+](https://launchpad.net/ubuntu/xenial/+package/libboost-all-dev)
* [libepoxy 1.3+](https://launchpad.net/ubuntu/xenial/+source/libepoxy)

These additional dependencies can be installed to support loading images as inputs:

* BMP, PNG, JPG, TGA, DDS, PSD, HDR: [libsoil-dev](https://launchpad.net/ubuntu/xenial/+package/libsoil-dev)
* JPG (including progressive): [libjpeg-dev](https://launchpad.net/ubuntu/xenial/+package/libjpeg-dev)
* OpenEXR (half precision): [libopenexr-dev](https://launchpad.net/ubuntu/xenial/+package/libopenexr-dev)

If you want the OpenGL exceptions to include the backtrace, the following
dependencies are also required:

* [libunwind-dev](https://launchpad.net/ubuntu/xenial/+package/libunwind8-dev)

You can then compile and install the library using CMake:

```bash
# Configure
mkdir -p build && cd build
cmake ..

# Compile
cmake --build .
# If using make on a Unix system
#cmake --build . -- -j$(nproc)

# Install to /usr/local (or CMAKE_INSTALL_PREFIX)
sudo cmake -P cmake_install.cmake
```

## Building the Debian/Ubuntu packages from source

To build the library on the development machine (needed for running tests) the
provided `Makefile` should be used. See `debian/control` for the up-to-date list
of build dependencies.

The packages for Ubuntu Xenial and Debian Stretch can be built using
[sbuild](https://wiki.debian.org/sbuild) and a suitably setup
[schroot](https://wiki.debian.org/Schroot). Here is a reminder on how to setup
such an environment (Debian Stretch instructions, based on provided links).

```bash
# Note: replace /disc/schroot with the location of the chroot you chose

# Debian Stretch amd64
sudo sbuild-createchroot --include=eatmydata,ccache,gnupg stretch /disc/schroot/stretch-amd64-sbuild http://deb.debian.org/debian

# Ubuntu Xenial amd64
sudo sbuild-createchroot --include=eatmydata,ccache,gnupg xenial /disc/schroot/xenial-amd64-sbuild http://archive.ubuntu.com/ubuntu/
```

Once the sbuild environment is setup, the Makefile can be used as follows.

```bash
# Build all Xenial packages (amd64, i386, source)
make xenial

# Build Debian Stretch amd64
make stretch-amd64

# The result can be found in ../libshadertoy-{version}-{distribution}-{git-revision}
```

The built packages are tested by the package building script using
`autopkgtest`. Note that at this time, only the schroot method has been working.
Here are the steps to build a working schroot for autopkgtest, using the schroot
created for building the packages.

```bash
# Switch into chroot
sudo schroot -c stretch-amd64-sbuild

# You need to install the same GL driver as the host, here for Nvidia
#  Enable non-free
sed -i 's/main$/main contrib non-free/' /etc/apt/sources.list
#  Install driver
apt-get update
apt-get install -y nvidia-driver

# You can also install the build dependencies beforehand to speed up the
# compilation
apt-get install -y build-essential libsoil-dev libjpeg-dev libepoxy-dev \
    libgl1-mesa-dev libglm-dev libopenexr-dev libunwind-dev doxygen doxygen-latex \
    graphviz pkg-config cmake git ca-certificates

# Leave the chroot
exit
```

