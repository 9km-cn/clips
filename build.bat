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
cmake -G "Visual Studio 15 Win64" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

rem home
cd ..

pause
