#!/bin/bash

# Find out which compiler to use
# Prefer clang-3.9, then 3.8, then GCC

if [ -z "$CC" ]; then
    if [ -f '/usr/bin/clang-3.9' ]; then
        if [ -f '/usr/bin/clang-3.8' ]; then
            export CC='/usr/bin/clang-3.8'
            export CXX='/usr/bin/clang++-3.8'
        else
            echo "Clang 3.9 is not supported, please install Clang 3.8" >&2
            echo "Aborting configration" >&2
            exit 2
        fi
    else
        # Check CC is version 4 or 5
        VERSION=$(/usr/bin/cc --version | head -1 | cut -d' ' -f4)
        if [[ "$VERSION" == '5'* ]]; then
            export CC='/usr/bin/cc'
            export CXX='/usr/bin/c++'
        else
            echo "Unsupported /usr/bin/cc version: $VERSION" >&2
            echo "Aborting configuration" >&2
            exit 2
        fi
    fi
fi

echo "Using $CC as the compiler" >&2

# Prepare the build directory

mkdir -p build
cd build

# Remove previous cache
[ -f 'CMakeCache.txt' ] && rm -f 'CMakeCache.txt'

# Configure
cmake "$@" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    ..

echo "Configuring complete, to compile now you can run the following commands:" >&2
echo "  cd build" >&2
echo "  make -j9" >&2
