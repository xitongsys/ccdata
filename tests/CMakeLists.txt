# 添加简单测试可执行文件
add_executable(basic_test test.cc)

# 链接主库
target_link_libraries(basic_test PRIVATE ccdata)

# 注册测试用例
add_test(
    NAME BasicFunctionalityTest   # 测试名称
    COMMAND basic_test            # 要执行的命令
)


set_property(TARGET basic_test PROPERTY CXX_STANDARD 20)

