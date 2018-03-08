#!/bin/bash

# Get the build directory
LIBDIRECTORY="$(pwd)"

# Grab the current version number from the rules file
LIBVERSION=$(head -n 1 debian/changelog | awk '{gsub("[()]","",$2); print $2}')

echo "[==== BUILDING v$LIBVERSION ====]" >&2

# We need to be in the parent directory
cd "$(dirname "$BASH_SOURCE")/.."

FAILED_TESTS=""

DISTRIBUTIONS=( stretch xenial )

if [ -n "$1" ]; then
	DISTRIBUTIONS=( "$@" )
fi

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

for DISTRIBUTION in "${DISTRIBUTIONS[@]}"; do
	rm -rf libshadertoy-$LIBVERSION-$DISTRIBUTION
	mkdir -p libshadertoy-$LIBVERSION-$DISTRIBUTION

	for ARCH in amd64 i386; do
		echo "[==== BUILDING $DISTRIBUTION-$ARCH ====]" >&2
		if [ "$ARCH" = "amd64" ]; then
			ARCHALL_ARG='--arch-all'
		else
			ARCHALL_ARG='--no-arch-all'
		fi

		(cd $LIBDIRECTORY && sbuild --no-apt-update --no-apt-upgrade \
									--no-apt-clean --resolve-alternatives \
									-d $DISTRIBUTION \
									--arch $ARCH \
									$ARCHALL_ARG \
									--append-to-version "-$(version_suffix $DISTRIBUTION)"
		)

		RESULT="$?"
		if [ "$RESULT" -ne "0" ]; then
			echo "[==== BUILD FAILED FOR $DISTRIBUTION-$ARCH ====]" >&2
			exit "$RESULT"
		fi
		echo "[==== MOVING ARTIFACTS $DISTRIBUTION-$ARCH ====]" >&2
		find . -maxdepth 1 -type f -exec mv {} libshadertoy-$LIBVERSION-$DISTRIBUTION/ \;
		find . -maxdepth 1 -type l -exec mv {} libshadertoy-$LIBVERSION-$DISTRIBUTION/ \;
		echo "[==== TESTING ARTIFACTS $DISTRIBUTION-$ARCH ====]" >&2
		(cd $LIBDIRECTORY &&
			autopkgtest ../libshadertoy-$LIBVERSION-$DISTRIBUTION/libshadertoy*-$(version_suffix $DISTRIBUTION)_$ARCH.deb \
				../libshadertoy-$LIBVERSION-$DISTRIBUTION/libshadertoy*-$(version_suffix $DISTRIBUTION)_all.deb \
				-- schroot $DISTRIBUTION-$ARCH-sbuild)
		if [ "$?" -ne "0" ]; then
			echo "[==== TESTS FAILED FOR $DISTRIBUTION-$ARCH ====]" >&2
			FAILED_TESTS="${FAILED_TESTS} ${DISTRIBUTION}-${ARCH}"
			#exit $?
		fi
	done
done

echo "[==== DONE ====]" >&2

if [ -n "$FAILED_TESTS" ]; then
	echo "[==== TESTS HAVE FAILED FOR ====]" >&2
	for DA in $FAILED_TESTS; do
		echo "==> $DA" >&2
	done
	exit 1
fi

exit 0
