#!/bin/bash

while getopts ":d" opt; do
	case "$opt" in
		d)
			# Build deb using docker
			docker build -t libshadertoy .

			# Run container
			docker run --rm -it -v $(pwd)/..:/app -v $HOME/.gnupg:/root/.gnupg \
				libshadertoy:latest

			# Quit
			exit $?
		;;
	esac
done

# Else, default build
./configure.sh "$@"
cd build
make -j9
