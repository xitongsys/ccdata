add_executable(test_array test_array.cc)
target_link_libraries(test_array PRIVATE ccdata)
add_test(NAME TestArray  COMMAND test_array)
set_property(TARGET test_array PROPERTY CXX_STANDARD 20)

add_executable(test_series test_series.cc)
target_link_libraries(test_series PRIVATE ccdata)
add_test(NAME TestSeries COMMAND test_series)
set_property(TARGET test_series PROPERTY CXX_STANDARD 20)

add_executable(test_frame test_frame.cc)
target_link_libraries(test_frame PRIVATE ccdata)
add_test(NAME TestFrame COMMAND test_frame)
set_property(TARGET test_frame PROPERTY CXX_STANDARD 20)

add_executable(test_concat test_concat.cc)
target_link_libraries(test_concat PRIVATE ccdata)
add_test(NAME TestConcat COMMAND test_concat)
set_property(TARGET test_concat PROPERTY CXX_STANDARD 20)


add_executable(test01 test01.cc)
target_link_libraries(test01 PRIVATE ccdata)
add_test(NAME Test01 COMMAND test01)
set_property(TARGET test01 PROPERTY CXX_STANDARD 20)

