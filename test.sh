#!/bin/bash

SKIP_BUILD=0
while getopts ":s" opt; do
	case "$opt" in
		s)
			SKIP_BUILD=1
			;;
	esac
done

if [ "$SKIP_BUILD" = "0" ]; then
	set -e
	(cd build && cmake -DSHADERTOY_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=$(pwd)/install/usr/local .. && make -j$(( $(nproc) + 1 )) && cmake -P cmake_install.cmake)
	set +e
fi

FAILED_TESTS=0
FAILED_TEST_NAMES=()
DEFAULT_TESTS=debian/tests/*

for TEST_FILE in ${@:-$DEFAULT_TESTS}; do
	if [ -x "$TEST_FILE" ] && [[ "$TEST_FILE" != *".sh" ]]; then
		TEST_NAME=$(basename "$TEST_FILE")

		echo "[==== RUNNING TEST: $TEST_NAME ====]" >&2

		# Create temporary directory
		TEST_DIR=`mktemp -d`
		export AUTOPKGTEST_TMP="$TEST_DIR"

		# Setup special vars
		export LIBSHADERTOY_DIR=$(pwd)/build/install/usr/local/share/shadertoy
		export CMAKE_EXTRA_OPTS="-DCMAKE_PREFIX_PATH=$(pwd)/build/install/usr/local -DCMAKE_MODULE_PATH=$(pwd)/modules -DCMAKE_BUILD_TYPE=Debug"

		# Execute test
		"$TEST_FILE"
		TEST_RESULT=$?

		if [ "$TEST_RESULT" -eq 0 ]; then
			echo "[==== TEST: $TEST_NAME: PASSED ====]" >&2
			# Cleanup
			rm -rf "$TEST_DIR"
		else
			echo "[==== TEST: $TEST_NAME: FAILED ($TEST_RESULT) ====]" >&2

			if [ "$TEST_RESULT" -eq 1 ]; then
				FAILED_TESTS=$(( FAILED_TESTS + 1 ))
				FAILED_TEST_NAMES+=("$TEST_NAME")
			elif [ "$TEST_RESULT" -eq 2 ]; then
				echo "  - Considered non-critical by test script" >&2
			fi
		fi
	fi
done

if [ $FAILED_TESTS -gt 0 ]; then
	echo "[==== TEST: SUMMARY OF FAILED TESTS ====]" >&2
	for FAILED_TEST_NAME in "${FAILED_TEST_NAMES[@]}"; do
		echo "  - $FAILED_TEST_NAME" >&2
	done
	exit 1
else
	echo "[==== TEST: ALL TESTS PASSED ====]" >&2
	exit 0
fi

