rm -Recurse -Force build_win
mkdir build_win
cd build_win
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE='C:\app\vcpkg\scripts\buildsystems\vcpkg.cmake' -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:\app\ccdata"  ..
cmake --build .
