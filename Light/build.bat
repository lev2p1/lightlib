@echo off
setlocal

set PROJECT_NAME=light
set BUILD_DIR=build
set SOURCE_DIR=%~dp0
set GENERATOR="Visual Studio 17 2022"
set CONFIG=Release
if defined VCPKG_ROOT (
    set VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
) else (
    set VCPKG_TOOLCHAIN=C:\Tools\vcpkg\scripts\buildsystems\vcpkg.cmake
)

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

cd /d "%BUILD_DIR%"

cmake -G %GENERATOR% -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" %SOURCE_DIR%
cmake --build . --config %CONFIG%

if exist "%SOURCE_DIR%.env" (
    copy "%SOURCE_DIR%.env" "%CONFIG%\"
    echo File .env copied to %CONFIG%\%PROJECT_NAME%.exe
) else (
    echo Warning: File .env not found in source directory.
)

if exist "%CONFIG%\%PROJECT_NAME%.exe" (
    echo Running %PROJECT_NAME%.exe...
    "%CONFIG%\%PROJECT_NAME%.exe"
) else (
    echo The file %PROJECT_NAME%.exe was not found.
)

cd /d "%SOURCE_DIR%"