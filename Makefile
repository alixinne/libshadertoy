ALL_DISTS=xenial stretch
ALL_XENIAL_ARCHS=xenial-amd64 xenial-i386 xenial-source
ALL_STRETCH_ARCHS=stretch-amd64 stretch-i386 stretch-source
ALL_PKGS=$(ALL_XENIAL_ARCHS) $(ALL_STRETCH_ARCHS)

IGNORE_TEST_FAILURES?=1
SKIP_TESTS?=

all: $(ALL_DISTS)

xenial: $(ALL_XENIAL_ARCHS)

stretch: $(ALL_STRETCH_ARCHS)

$(ALL_PKGS):
	./buildpackage.sh $@

.PHONY: all $(ALL_DISTS) $(ALL_PKGS)

