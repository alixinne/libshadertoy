# Known build distributions
ALL_DISTS=bionic stretch buster

# Known package types
PKG_TYPES=amd64 i386 source

# Possible combinations of distributions-types
ALL_BIONIC_ARCHS=$(patsubst %,bionic-%,$(PKG_TYPES))
ALL_STRETCH_ARCHS=$(patsubst %,stretch-%,$(PKG_TYPES))
ALL_BUSTER_ARCHS=$(patsubst %,buster-%,$(PKG_TYPES))
ALL_PKGS=$(ALL_BIONIC_ARCHS) $(ALL_STRETCH_ARCHS) $(ALL_BUSTER_ARCHS)

# Test settings
IGNORE_TEST_FAILURES?=
SKIP_TESTS?=

# CI settings
VERS=$(shell head -n 1 debian/changelog | awk '{gsub("[()]","",$$2);print $$2}')
OS_DIST=$(shell . /etc/os-release && test -n "$$VERSION_CODENAME" && echo -n "$$VERSION_CODENAME" || \
	   test -n "$$VERSION" && echo -n "$$VERSION" | cut -d' ' -f2 | sed 's/[()]//g' || \
	   echo -n "$$PRETTY_NAME" | cut -d' ' -f3 | cut -d/ -f1)
export OS_DIST
CI_BUILD_TYPE?=ci-src

GIT_PREFIX=$(shell git rev-parse --short HEAD)
export GIT_PREFIX

all: $(ALL_DISTS)

bionic: $(ALL_BIONIC_ARCHS)

stretch: $(ALL_STRETCH_ARCHS)

buster: $(ALL_BUSTER_ARCHS)

$(ALL_PKGS):
	./buildpackage.sh $@

ci: $(CI_BUILD_TYPE)

# ci-src builds the project from source
ci-src:
	./buildpackage.sh ci-src

# ci-pkg builds packages for the current distribution, on supported hosts
ci-pkg: $(patsubst %,$(OS_DIST)-%,$(PKG_TYPES))
	tar -cjvf libshadertoy-$(VERS)-$(OS_DIST)-$(GIT_PREFIX).tar.bz2 ../libshadertoy-$(VERS)-$(OS_DIST)-$(GIT_PREFIX)

gl:
	./buildpackage.sh $@
	mv ../libshadertoy-$(VERS)-$(OS_DIST)-$(GIT_PREFIX)/*.deb .

.PHONY: all ci-src ci-pkg $(ALL_DISTS) $(ALL_PKGS)

