# Using the examples

Some example programs are included in the library source and package
distributions to help you getting started using the library. They highlight some
features that are desirable for users of the library.

The examples can be built as follows (assuming the correct version of
libshadertoy is already installed.

```bash
# Copy the samples to a working directory
cp -r /usr/share/shadertoy/examples shadertoy-example
cd shadertoy-example

# With libshadertoy-dev and libshadertoy1, install additional dependencies
sudo apt-get install libglfw3-dev cmake g++ pkg-config libcurl4-openssl-dev \
    libjsoncpp-dev

# Create a build directory and make the project
mkdir -p build && cd build
cmake ..
make -j

# Run the examples
./src/00-build/example00-build
./src/10-gradient/example10-gradient
# etc.
```

