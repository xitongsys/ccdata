cmake_minimum_required (VERSION 3.8)
project ("ccdata")

add_compile_options(/W0)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cc")

add_library(ccdata STATIC ${SOURCES})
target_include_directories(ccdata PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)


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

set_property(TARGET ccdata PROPERTY CXX_STANDARD 20)


enable_testing()
add_subdirectory("tests")