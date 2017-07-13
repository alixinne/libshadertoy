#!/bin/bash

if [ "$EUID" -eq 0 ]; then
	# Update sources
	apt-get update -qq

	# Install build dependencies
	apt-get -y build-dep .
fi

if [ "$EUID" -eq 0 ] && [ -n "$BUILD_UID" ]; then
	useradd -g "$BUILD_GID" -u "$BUILD_UID" buildusr
	exec sudo -u buildusr "$0"
fi

# Build package
debuild
