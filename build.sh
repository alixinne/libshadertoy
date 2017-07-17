#!/bin/bash

# Safe mode
set -e

build_docker () {
	# Build deb using docker
	docker build -t libshadertoy/$OPTARG -f Dockerfile.$OPTARG .

	# Run container
	docker run --rm -it -v $(pwd)/..:/app -v $HOME/.gnupg:/root/.gnupg-src:ro \
		libshadertoy/$OPTARG:latest

	# Quit
	exit $?
}

while getopts ":d:" opt; do
	case "$opt" in
		d)
			build_docker
		;;
		:)
			if [ "$OPTARG" = "d" ]; then
				OPTARG=debian
				build_docker
			fi
		;;
	esac
done

# Else, default build
./configure.sh "$@"
cd build
make -j9
