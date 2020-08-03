@echo off

rem build dir
if not exist "build" (
    md "build"
)
if not exist "build" (
    md "build"
)

rem build
cd "build"
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release

rem home
cd ..

pause
