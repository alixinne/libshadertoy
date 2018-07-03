#!/bin/bash

export LIBSHADERTOY_TESTING=1

while getopts ":n:a:" opt; do
	case "$opt" in
		a)
			TEST_ARGS="$OPTARG"
			;;
		n)
			TEST_NAME="$OPTARG"
			;;
	esac
done

if [ "x$TEST_NAME" = "x" ]; then
	echo "Usage: ./st-autotest.sh -n 00-build" >&2
	exit 50
fi

if [ "x$LIBSHADERTOY_DIR" = "x" ]; then
	# Guess libshadertoy directory
	LIBSHADERTOY_DIR="$(dirname ${BASH_SOURCE[0]})/../.."

	if ! [ -d "$LIBSHADERTOY_DIR/examples" ]; then
		LIBSHADERTOY_DIR="/usr/share/shadertoy"
	fi

	if ! [ -d "$LIBSHADERTOY_DIR/examples" ]; then
		LIBSHADERTOY_DIR="/usr/local/share/shadertoy"
	fi
	
	if ! [ -d "$LIBSHADERTOY_DIR/examples" ]; then
		echo "Failed to locate libshadertoy example dir" >&2
		exit 1
	fi

	export LIBSHADERTOY_DIR
fi

if [ "x$AUTOPKGTEST_TMP" = "x" ]; then
	export AUTOPKGTEST_TMP="$(mktemp -d)"
	cleanup () {
		cd "$LIBSHADERTOY_DIR"
		rm -rf "$AUTOPKGTEST_TMP"
	}
	trap cleanup EXIT
fi

echo "Using $AUTOPKGTEST_TMP as a temporary directory" >&2

# Copy the examples
cp -r "$LIBSHADERTOY_DIR/examples" "$AUTOPKGTEST_TMP"
cd "$AUTOPKGTEST_TMP/examples"

# Configure
set -o pipefail

mkdir build
cd build
cmake $CMAKE_EXTRA_OPTS .. || exit 1

# Build
make -j$(( $(nproc) + 1 )) VERBOSE=1 example$TEST_NAME || exit 1

$ST_AUTOTEST_LAUNCH_ARGS ./src/$TEST_NAME/example$TEST_NAME $TEST_ARGS
RESULT="$?"

if [ "$RESULT" -eq 2 ]; then
	# 2 is assumed to be returned on failure to create a glfw context
	# which is fine on a headless server
	if [ "x$DISPLAY" = "x" ]; then
		exit 0
	else
		exit 2
	fi
else
	exit "$RESULT"
fi



