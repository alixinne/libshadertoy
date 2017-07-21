#!/bin/bash

# Grab the current version number from the directory
LIBVERSION=$(basename "$(pwd)" | sed 's/libshadertoy-//')

echo "[==== BUILDING v$LIBVERSION ====]" >&2

# We need to be in the parent directory
cd "$(dirname "$BASH_SOURCE")/.."

for DISTRIBUTION in stretch xenial; do
	rm -rf libshadertoy-$LIBVERSION-$DISTRIBUTION
	mkdir -p libshadertoy-$LIBVERSION-$DISTRIBUTION

	for ARCH in i386 amd64; do
		echo "[==== BUILDING $DISTRIBUTION-$ARCH ====]" >&2
		(cd libshadertoy-$LIBVERSION && sbuild -d $DISTRIBUTION --arch $ARCH)
		if [ "$?" -ne "0" ]; then
			echo "[==== BUILD FAILED FOR $DISTRIBUTION-$ARCH ====]" >&2
			exit $?
		fi
		echo "[==== MOVING ARTIFACTS $DISTRIBUTION-$ARCH ====]" >&2
		find . -maxdepth 1 -type f -exec mv {} libshadertoy-$LIBVERSION-$DISTRIBUTION/ \;
		find . -maxdepth 1 -type l -exec mv {} libshadertoy-$LIBVERSION-$DISTRIBUTION/ \;
		echo "[==== TESTING ARTIFACTS $DISTRIBUTION-$ARCH ====]" >&2
		(cd libshadertoy-$LIBVERSION &&
			autopkgtest ../libshadertoy-$LIBVERSION-$DISTRIBUTION/libshadertoy*_$ARCH.deb \
				../libshadertoy-$LIBVERSION-$DISTRIBUTION/libshadertoy-dev_${LIBVERSION}_all.deb \
				-- schroot $DISTRIBUTION-$ARCH-sbuild)
		if [ "$?" -ne "0" ]; then
			echo "[==== TESTS FAILED FOR $DISTRIBUTION-$ARCH ====]" >&2
			exit $?
		fi
	done
done

echo "[==== DONE ====]" >&2

