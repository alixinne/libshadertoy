#!/bin/bash

# Safe mode
set -e

# Else, default build
./configure.sh "$@"
cd build
if make -j9; then
	make DESTDIR=$(pwd)/install install
fi
