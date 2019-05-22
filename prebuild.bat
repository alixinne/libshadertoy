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

REM Bootstrap vcpkg
SET OLD_CD=%cd%
SET PATH=%PATH%;%VCPKG_ROOT%
cd %VCPKG_ROOT%
CALL .\bootstrap-vcpkg.bat

REM Upgrade libraries
vcpkg upgrade --no-dry-run

REM Install dependencies
vcpkg install soil libjpeg-turbo libepoxy glm
