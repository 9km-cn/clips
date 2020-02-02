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
cmake -G "Visual Studio 15 Win64" -DCMAKE_BUILD_TYPE=Release -DTEST=1 ..
cmake --build .

rem home
cd ..

pause
