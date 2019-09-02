REM Install pkg-config
pkg-config --version

IF %ERRORLEVEL% NEQ 0 (
	choco install -y pkgconfiglite
)

REM Install vcpkg
SET VCPKG_ROOT=%USERPROFILE%\vcpkg
SET VCPKG_DEFAULT_TRIPLET=x64-windows

REM Clone and preserve installed folder
robocopy %VCPKG_ROOT%\installed %USERPROFILE%\vcpkg-installed /MOVE /MIR /NP /NFL
robocopy %VCPKG_ROOT%\packages %USERPROFILE%\vcpkg-packages /MOVE /MIR /NP /NFL
git clone --depth=1 https://github.com/Microsoft/vcpkg.git %VCPKG_ROOT%
robocopy %USERPROFILE%\vcpkg-installed %VCPKG_ROOT%\installed /MOVE /MIR /NP /NFL
robocopy %USERPROFILE%\vcpkg-packages %VCPKG_ROOT%\packages /MOVE /MIR /NP /NFL

REM Bootstrap vcpkg
SET OLD_CD=%cd%
SET PATH=%PATH%;%VCPKG_ROOT%
cd %VCPKG_ROOT%
CALL .\bootstrap-vcpkg.bat

REM Set triplet file to release-only
copy %VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake %VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake.bak
ECHO set(VCPKG_BUILD_TYPE release) >>%VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake

REM Install release-only dependencies
vcpkg install openimageio glm

REM Restore triplet file
copy %VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake.bak %VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake

REM Install libepoxy (meson/ninja is broken for release-only)
vcpkg install libepoxy
