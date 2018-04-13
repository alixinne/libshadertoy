# Known build distributions
ALL_DISTS=xenial stretch

# Known package types
PKG_TYPES=amd64 i386 source

# Possible combinations of distributions-types
ALL_XENIAL_ARCHS=$(patsubst %,xenial-%,$(PKG_TYPES))
ALL_STRETCH_ARCHS=$(patsubst %,stretch-%,$(PKG_TYPES))
ALL_PKGS=$(ALL_XENIAL_ARCHS) $(ALL_STRETCH_ARCHS)

# Test settings
IGNORE_TEST_FAILURES?=1
SKIP_TESTS?=

# CI settings
VERS=$(shell head -n 1 debian/changelog | awk '{gsub("[()]","",$2);print $2')
OS_DIST=$(shell . /etc/os-release && test -n "$$VERSION_CODENAME" && echo -n "$$VERSION_CODENAME" || \
	   echo -n "$$VERSION" | cut -d' ' -f2 | sed 's/[()]//g')
CI_BUILD_TYPE?=ci-src
GIT_PREFIX=$(shell git rev-parse --short HEAD)

all: $(ALL_DISTS)

xenial: $(ALL_XENIAL_ARCHS)

stretch: $(ALL_STRETCH_ARCHS)

$(ALL_PKGS):
	./buildpackage.sh $@

ci: $(CI_BUILD_TYPE)

# ci-src builds the project from source
ci-src:
	./buildpackage.sh ci-src

# ci-pkg builds packages for the current distribution, on supported hosts
ci-pkg: $(patsubst %,$(OS_DIST)-%,$(PKG_TYPES))
	tar -cjvf libshadertoy-$(VERS)-$(OS_DIST)-$(GIT_PREFIX).tar.bz2 ../libshadertoy-$(VERS)-$(OS_DIST)-$(GIT_PREFIX)

.PHONY: all ci-src ci-pkg $(ALL_DISTS) $(ALL_PKGS)

