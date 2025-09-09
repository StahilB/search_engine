cmake_minimum_required(VERSION 3.16)
project(search_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(PROJ_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

set(ENGINE_SOURCES
  ${CMAKE_SOURCE_DIR}/src/ConverterJSON.cpp
  ${CMAKE_SOURCE_DIR}/src/InvertedIndex.cpp
  ${CMAKE_SOURCE_DIR}/src/SearchServer.cpp
)
add_library(engine_lib ${ENGINE_SOURCES})
target_include_directories(engine_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(engine_lib PUBLIC nlohmann_json::nlohmann_json)

add_executable(search_engine ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_include_directories(search_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(search_engine PRIVATE engine_lib)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(search_engine_tests ${CMAKE_SOURCE_DIR}/tests/tests.cpp)
target_include_directories(search_engine_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(search_engine_tests PRIVATE engine_lib gtest_main)

include(GoogleTest)
gtest_discover_tests(search_engine_tests)







