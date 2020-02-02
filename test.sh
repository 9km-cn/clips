
if [ ! -d "build" ]; then
  mkdir build
fi

if [ -d "build" ]; then
  rm -rf build
fi
mkdir build

cd build
cmake -DCMAKE_BUILD_TYPE=Release -DTEST=1 ..
make -j8
#cpack --config CPackConfig.cmake

cd ..