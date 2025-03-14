cmake_minimum_required (VERSION 3.8)

add_compile_options(/W0)


project ("ccdata")
#add_library(ccdata )
add_executable(test 
    src/pandas/datetime.cc
    src/pandas/util.cc
    src/test.cc)


include_directories(include)

# DataFrame
#include_directories(third_party/DataFrame/include)
#add_subdirectory(third_party/DataFrame)
#target_link_libraries(qnn PUBLIC DataFrame)


set_property(TARGET test PROPERTY CXX_STANDARD 20)
