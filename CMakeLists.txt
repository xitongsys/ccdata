cmake_minimum_required (VERSION 3.8)
project ("ccdata")

include_directories("include")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -fPIC -w")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -fsanitize=address -fPIC -w")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /W4 /arch:AVX2 /W0")
    include_directories("C:/app/boost/include/boost-1_87")
endif()


file(GLOB_RECURSE SOURCES "src/*.cc")
add_library(ccdata STATIC ${SOURCES})
set_property(TARGET ccdata PROPERTY CXX_STANDARD 20)
target_include_directories(ccdata PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(
    TARGETS ccdata
    EXPORT ccdataTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

install(
    DIRECTORY include/ 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
    FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.tcc"
)

enable_testing()
add_subdirectory("tests")