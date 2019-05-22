ARG base_image=debian:buster
FROM $base_image
ENV DEBIAN_FRONTEND=noninteractive
RUN mkdir /build
WORKDIR /build
COPY debian ./debian
RUN mv /etc/apt/sources.list /etc/apt/sources.list.save && \
    awk '/^deb /{print;sub(/^deb/,"deb-src",$0);print}' /etc/apt/sources.list.save >/etc/apt/sources.list && \
    apt-get update -qq && \
    apt-get install --no-install-recommends -y \
        git \
        ca-certificates \
        curl \
        devscripts \
        equivs && \
    mk-build-deps --install --remove ./debian/control -t 'apt-get -o Debug::pkgProblemResolver=yes --no-install-recommends -y'
