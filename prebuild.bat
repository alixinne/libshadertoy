REM Install pkg-config
pkg-config --version

IF %ERRORLEVEL% NEQ 0 (
	choco install -y pkgconfiglite
)

REM Install vcpkg
SET VCPKG_ROOT=%USERPROFILE%\vcpkg
SET VCPKG_DEFAULT_TRIPLET=x64-windows

IF NOT EXIST %VCPKG_ROOT%\vcpkg.exe (
	md %VCPKG_ROOT%
	git clone --depth=1 https://github.com/Microsoft/vcpkg.git %VCPKG_ROOT%
) ELSE (
	git -C %VCPKG_ROOT% fetch
	git -C %VCPKG_ROOT% reset --hard origin/master
)

copy %VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake %VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake.bak
ECHO set(VCPKG_BUILD_TYPE release) >>%VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake

REM Bootstrap vcpkg
SET OLD_CD=%cd%
SET PATH=%PATH%;%VCPKG_ROOT%
cd %VCPKG_ROOT%
CALL .\bootstrap-vcpkg.bat

REM Upgrade libraries
vcpkg upgrade --no-dry-run

REM Install release-only dependencies
vcpkg install openimageio glm

REM Restore triplet file
copy %VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake.bak %VCPKG_ROOT%\triplets\%VCPKG_DEFAULT_TRIPLET%.cmake

REM Install libepoxy (meson/ninja is broken for release-only)
vcpkg install libepoxy

REM Cleanup
rd /Q /S %VCPKG_ROOT%\buildtrees
