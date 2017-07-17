#!/bin/bash

for TEST_FILE in debian/tests/*; do
	if [ -x "$TEST_FILE" ]; then
		TEST_NAME=$(basename "$TEST_FILE")

		echo "[==== RUNNING TEST: $TEST_NAME ====]" >&2

		# Create temporary directory
		TEST_DIR=`mktemp -d`
		export AUTOPKGTEST_TMP="$TEST_DIR"

		# Setup special vars
		export LIBSHADERTOY_DIR=$(pwd)/build/install/usr/local/share/shadertoy
		export CMAKE_EXTRA_OPTS="-DCMAKE_PREFIX_PATH=$(pwd)/build/install/usr/local -DCMAKE_MODULE_PATH=$(pwd)/modules"

		# Execute test
		"$TEST_FILE"
		TEST_RESULT=$?

		if [ "$TEST_RESULT" -eq 0 ]; then
			echo "[==== TEST: $TEST_NAME: PASSED ====]" >&2
		else
			echo "[==== TEST: $TEST_NAME: FAILED ($TEST_RESULT) ====]" >&2
		fi

		# Cleanup
		rm -rf "$TEST_DIR"
	fi
done
