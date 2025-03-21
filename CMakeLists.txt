cmake_minimum_required (VERSION 3.8)
project ("ccdata")

add_compile_options(/W0)





set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /Ot /Oy /GL /GS- /W4 /WX /DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /OPT:REF")



#add_library(ccdata )
add_executable(test 
    src/pandas/datetime.cc
    src/pandas/util.cc
    src/pandas/ops.cc
    src/pandas/string.cc
    src/test.cc)


include_directories(include)

# DataFrame
#include_directories(third_party/DataFrame/include)
#add_subdirectory(third_party/DataFrame)
#target_link_libraries(qnn PUBLIC DataFrame)


set_property(TARGET test PROPERTY CXX_STANDARD 20)

