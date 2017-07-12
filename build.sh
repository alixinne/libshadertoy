#!/bin/sh
./configure.sh "$@"
cd build
make -j9
