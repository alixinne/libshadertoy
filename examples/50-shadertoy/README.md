# libshadertoy - 50-shadertoy

This example implements a desktop version of ShaderToy using the implemented
functionality in libshadertoy. Note that in order to support progressive JPEG
(not supported by SOIL), ImageMagick is required to convert JPEG images to PNG.

## Example of invocation

```bash
./example50-shadertoy -i ltfSDl -a fdnKWn
```

## Tested shaders

* [ltfSDl](https://www.shadertoy.com/view/ltfSDl): no sound support
* [ldl3z2](https://www.shadertoy.com/view/ldl3z2)
* [XsfXW8](https://www.shadertoy.com/view/XsfXW8): no keyboard support
* [lljSDy](https://www.shadertoy.com/view/lljSDy)
* [ldlSzX](https://www.shadertoy.com/view/ldlSzX): no keyboard support
* [4sl3RX](https://www.shadertoy.com/view/4sl3RX): shaking seems stronger on desktop?
* [MsfXzM](https://www.shadertoy.com/view/MsfXzM): all black, no keyboard support
* [4tfSzS](https://www.shadertoy.com/view/4tfSzS)
* [4dGGz1](https://www.shadertoy.com/view/4dGGz1): no keyboard support
* [lt33zl](https://www.shadertoy.com/view/lt33zl)

## Dependencies

* imagemagick
* libcurl4-openssl-dev
* libjsoncpp-dev
* libboost-all-dev
* libglfw3-dev
* cmake
* git
* g++
* ca-certificates
* pkg-config

## Copyright

libshadertoy - Vincent Tavernier <vince.tavernier@gmail.com>
