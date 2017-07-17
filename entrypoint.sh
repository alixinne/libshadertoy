#!/bin/bash

if [ "$EUID" -eq 0 ]; then
	if [ "$SHADERTOY_ENTRYPOINT" != "ubuntu" ]; then
		# Update sources
		apt-get update -qq

		# Install build dependencies
		apt-get -y build-dep .
	fi

	cp -r /root/.gnupg-src /root/.gnupg
fi

if [ "$EUID" -eq 0 ] && [ -n "$BUILD_UID" ]; then
	useradd -g "$BUILD_GID" -u "$BUILD_UID" buildusr
	exec sudo -u buildusr "$0"
fi

if [ "$SHADERTOY_ENTRYPOINT" = "ubuntu" ]; then
	export DEBSIGN_PROGRAM=gpg2

	apt-get update -qq
	apt-get install -y gnupg2
	mv /usr/bin/gpg /usr/bin/gpg1
	ln -fs /usr/bin/gpg2 /usr/bin/gpg
fi

# Build package
debuild
