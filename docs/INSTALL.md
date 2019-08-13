# Installing

There are several options for installing libshadertoy.

## Install the Debian/Ubuntu packages

A package repository is maintained for Debian Buster (10) and Ubuntu Bionic (18.04 LTS).
It can be used to install a pre-built version of the library with all its features:

```bash
# Get repository GPG key
sudo apt-key adv --recv-keys --keyserver pool.sks-keyservers.net --recv 0xF35C413E

# Add repository to apt config
#  Debian Buster
sudo sh -c 'echo "deb https://dl.bintray.com/vtavernier/libshadertoy buster main" >/etc/apt/sources.list.d/libshadertoy-apt.list'
#  Ubuntu Bionic (18.04)
sudo sh -c 'echo "deb https://dl.bintray.com/vtavernier/libshadertoy bionic main" >/etc/apt/sources.list.d/libshadertoy-apt.list'

# Update and install
sudo apt update
sudo apt install libshadertoy-dev
```

## Build from source

To build the library from source, first clone the repository as well as its
dependencies:

```bash
git clone --recursive https://github.com/vtavernier/libshadertoy.git
```

In order to compile the project, the following tools and libraries are required:

* OpenGL 4.5
* C++17-enabled compiler (GCC 7 or Clang 5)
* [CMake 3.1+](https://launchpad.net/ubuntu/bionic/+source/cmake)
* [libepoxy 1.3+](https://launchpad.net/ubuntu/bionic/+source/libepoxy)

*Note: GCC 5 is not supported due to a bug fixed in v6: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67371*

These additional dependencies can be installed to support loading images as inputs:

* OpenImageIO: [libopenimageio-dev](https://launchpad.net/ubuntu/bionic/+package/libopenimageio-dev)

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

The packages for Ubuntu Bionic and Debian Buster can be built using
[sbuild](https://wiki.debian.org/sbuild) and a suitably setup
[schroot](https://wiki.debian.org/Schroot). Here is a reminder on how to setup
such an environment (Debian Buster instructions, based on provided links).

```bash
# Note: replace /disc/schroot with the location of the chroot you chose

# Debian Buster amd64
sudo sbuild-createchroot --include=eatmydata,ccache,gnupg buster /disc/schroot/buster-amd64-sbuild http://deb.debian.org/debian

# Ubuntu Bionic amd64
sudo sbuild-createchroot --include=eatmydata,ccache,gnupg bionic /disc/schroot/bionic-amd64-sbuild http://archive.ubuntu.com/ubuntu/
```

Once the sbuild environment is setup, the Makefile can be used as follows.

```bash
# Build all Bionic packages (amd64, i386, source)
make bionic

# Build Debian Buster amd64
make buster-amd64

# The result can be found in ../libshadertoy-{version}-{distribution}-{git-revision}
```

The built packages are tested by the package building script using
`autopkgtest`. Note that at this time, only the schroot method has been working.
Here are the steps to build a working schroot for autopkgtest, using the schroot
created for building the packages.

```bash
# Switch into chroot
sudo schroot -c buster-amd64-sbuild

# You need to install the same GL driver as the host, here for Nvidia
#  Enable non-free
sed -i 's/main$/main contrib non-free/' /etc/apt/sources.list
#  Install driver
apt-get update
apt-get install -y nvidia-driver

# You can also install the build dependencies beforehand to speed up the
# compilation
apt-get install -y build-essential libopenimageio-dev \
    libgl1-mesa-dev libglm-dev doxygen doxygen-latex \
    graphviz pkg-config cmake git ca-certificates

# Leave the chroot
exit
```

