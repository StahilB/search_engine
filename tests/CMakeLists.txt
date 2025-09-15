add_executable(search_engine_tests tests.cpp)

target_include_directories(search_engine_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(search_engine_tests
  PRIVATE engine_lib gtest_main
)

include(GoogleTest)
gtest_discover_tests(search_engine_tests)
