rm -rvf build
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
#sudo cmake --install build

