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

add_executable(test_datetime test_datetime.cc)
target_link_libraries(test_datetime PRIVATE ccdata)
add_test(NAME TestDatetime COMMAND test_datetime)
set_property(TARGET test_datetime PROPERTY CXX_STANDARD 20)

add_executable(test01 test01.cc)
target_link_libraries(test01 PRIVATE ccdata)
add_test(NAME Test01 COMMAND test01)
set_property(TARGET test01 PROPERTY CXX_STANDARD 20)

add_executable(test_array_comprehensive test_array_comprehensive.cc)
target_link_libraries(test_array_comprehensive PRIVATE ccdata)
add_test(NAME TestArrayComprehensive COMMAND test_array_comprehensive)
set_property(TARGET test_array_comprehensive PROPERTY CXX_STANDARD 20)

add_executable(test_series_comprehensive test_series_comprehensive.cc)
target_link_libraries(test_series_comprehensive PRIVATE ccdata)
add_test(NAME TestSeriesComprehensive COMMAND test_series_comprehensive)
set_property(TARGET test_series_comprehensive PROPERTY CXX_STANDARD 20)

add_executable(test_rolling test_rolling.cc)
target_link_libraries(test_rolling PRIVATE ccdata)
add_test(NAME TestRolling COMMAND test_rolling)
set_property(TARGET test_rolling PROPERTY CXX_STANDARD 20)

