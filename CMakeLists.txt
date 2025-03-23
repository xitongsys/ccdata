cmake_minimum_required (VERSION 3.8)
project ("ccdata")

add_compile_options(/W0)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cc")

add_library(ccdata STATIC ${SOURCES})


# 头文件目录（供其他项目使用）
target_include_directories(${PROJECT_NAME}
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# 安装规则
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib      # 共享库
    ARCHIVE DESTINATION lib      # 静态库
    RUNTIME DESTINATION bin      # Windows DLL
    INCLUDES DESTINATION include
)

install(
    DIRECTORY include/ 
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

set_property(TARGET ccdata PROPERTY CXX_STANDARD 20)


enable_testing()
add_subdirectory("tests")