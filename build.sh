#!/bin/bash

if [ ! -d 'build_linux' ];then
    mkdir -p build_linux
fi

cd build_linux
cmake ..
cmake --build . --config Release
#cpack --config CPackConfig.cmake

cd ..
