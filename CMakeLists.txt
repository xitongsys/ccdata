cmake_minimum_required (VERSION 3.8)
project ("ccdata")

include_directories("include")

file(GLOB_RECURSE SOURCES "src/*.cc")
add_library(ccdata STATIC ${SOURCES})

set_property(TARGET ccdata PROPERTY CXX_STANDARD 20)

target_include_directories(ccdata PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include>)

# boost
find_package(Boost REQUIRED)
target_include_directories(ccdata PRIVATE ${Boost_INCLUDE_DIRS})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /arch:AVX2 /W0")

    # mimalloc
    find_package(mimalloc CONFIG REQUIRED)
    target_link_libraries(ccdata mimalloc)
    target_link_options(ccdata PRIVATE "/INCLUDE:mi_version")

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -fPIC -w")
    
endif()


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