REM Build library
SET VCPKG_ROOT=%USERPROFILE%\vcpkg
SET PATH=%PATH%;%VCPKG_ROOT%
SET VCPKG_DEFAULT_TRIPLET=x64-windows

REM Configure
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 15 2017 Win64" ..

REM Build library
cmake --build . --config Release -- /m

REM Build package
cmake --build . --config Release --target PACKAGE -- /m
