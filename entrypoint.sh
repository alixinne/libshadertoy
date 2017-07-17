#!/bin/bash

if [ "$SHADERTOY_ENTRYPOINT" != "ubuntu" ]; then
	# Update sources
	apt-get update -qq

	# Install build dependencies
	apt-get -y build-dep .
fi

cp -r /root/.gnupg-src /root/.gnupg

if [ "$SHADERTOY_ENTRYPOINT" = "ubuntu" ]; then
	export DEBSIGN_PROGRAM=gpg2

	apt-get update -qq
	apt-get install -y gnupg2
	mv /usr/bin/gpg /usr/bin/gpg1
	ln -fs /usr/bin/gpg2 /usr/bin/gpg
fi

# Build package
debuild
