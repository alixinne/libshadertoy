#!/bin/sh

# Update sources
apt-get update

# Install build dependencies
apt-get -y build-dep .

# Build package
debuild
