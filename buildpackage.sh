#!/bin/bash

# Get the build directory
LIBDIRECTORY="$(pwd)"

# Grab the current version number from the rules file
LIBVERSION=$(head -n 1 debian/changelog | awk '{gsub("[()]","",$2); print $2}')

# Temporary directory to work in
LIBTMPDIR=$(mktemp -d)
cp -r "$LIBDIRECTORY" "$LIBTMPDIR/libshadertoy"
cd "$LIBTMPDIR"

# Cleanup at exit
cleanup () {
	cd "$LIBDIRECTORY"
	echo "Cleanup $LIBTMPDIR" >&2
	rm -rf "$LIBTMPDIR"
}
trap cleanup EXIT

echo "[==== BUILDING v$LIBVERSION ====]" >&2

version_suffix () {
	if [ "$1" = "stretch" ]; then
		echo -n "debian9"
	elif [ "$1" = "xenial" ]; then
		echo -n "ubuntu16"
	elif [ "$1" = "trusty" ]; then
		echo -n "ubuntu14"
	else
		exit 1
	fi
}

DISTRIBUTION=$(echo -n "$1" | cut -d- -f1)
ARCH=$(echo -n "$1" | cut -d- -f2)

if [ "x$DISTRIBUTION" = "x" ] || [ "x$ARCH" = "x" ]; then
	exit 2
fi

echo "[==== BUILDING $DISTRIBUTION-$ARCH ====]" >&2
if [ "$ARCH" = "amd64" ]; then
	ARCHALL_ARG='--arch-all'
elif [ "$ARCH" = "i386" ]; then
	ARCHALL_ARG='--no-arch-all'
fi

DIR_ARCHSUFFIX="-$DISTRIBUTION"
TARGET_DIRECTORY=$LIBDIRECTORY/../libshadertoy-$LIBVERSION$DIR_ARCHSUFFIX
if [ "x$GIT_PREFIX" != "x" ]; then
	TARGET_DIRECTORY="${TARGET_DIRECTORY}-${GIT_PREFIX}"
fi

mkdir -p $TARGET_DIRECTORY

if [ "$ARCH" != "source" ]; then
	(cd libshadertoy && sbuild --no-apt-update --no-apt-upgrade \
								--no-apt-clean --resolve-alternatives \
								-d $DISTRIBUTION \
								--arch $ARCH \
								$ARCHALL_ARG \
								--append-to-version "-$(version_suffix $DISTRIBUTION)"
	)

	RESULT="$?"
	if [ "$RESULT" -ne "0" ]; then
		echo "[==== BUILD FAILED FOR $DISTRIBUTION-$ARCH ====]" >&2
		exit 1
	fi

	echo "[==== MOVING ARTIFACTS $DISTRIBUTION-$ARCH ====]" >&2
	CHANGES_FILE=libshadertoy_$LIBVERSION-$(version_suffix $DISTRIBUTION)_$ARCH.changes
	BUILD_ARTIFACTS=$(awk '/^Files:/{a=1;next}/^$/{a=0}{if(a)print $NF}' "$CHANGES_FILE")
	mv $BUILD_ARTIFACTS $CHANGES_FILE $TARGET_DIRECTORY

	if [ "x$SKIP_TESTS" = "x" ]; then
		echo "[==== TESTING ARTIFACTS $DISTRIBUTION-$ARCH ====]" >&2
		(cd libshadertoy &&
			autopkgtest $TARGET_DIRECTORY/libshadertoy*-$(version_suffix $DISTRIBUTION)_$ARCH.deb \
			$TARGET_DIRECTORY/libshadertoy*-$(version_suffix $DISTRIBUTION)_all.deb \
			-- schroot $DISTRIBUTION-$ARCH-sbuild)
		if [ "$?" -ne "0" ]; then
			echo "[==== TESTS FAILED FOR $DISTRIBUTION-$ARCH ====]" >&2
			if [ "x$IGNORE_TEST_FAILURES" = "x" ]; then
				exit 2
			fi
		fi
	fi
else
	(cd libshadertoy && dpkg-buildpackage -S -uc -us -rfakeroot -d \
										   --changes-option=-DDistribution=$DISTRIBUTION
	)

	RESULT="$?"
	if [ "$RESULT" -ne "0" ]; then
		echo "[==== BUILD FAILED FOR $DISTRIBUTION-$ARCH ====]" >&2
		exit 1
	fi

	echo "[==== MOVING ARTIFACTS $DISTRIBUTION-$ARCH ====]" >&2
	CHANGES_FILE=libshadertoy_${LIBVERSION}_$ARCH.changes
	BUILD_ARTIFACTS=$(awk '/^Files:/{a=1;next}/^$/{a=0}{if(a)print $NF}' "$CHANGES_FILE")
	mv $BUILD_ARTIFACTS $CHANGES_FILE $TARGET_DIRECTORY
fi

echo "[==== DONE ====]" >&2
exit 0

