add_library(engine_lib
  ConverterJSON.cpp
  InvertedIndex.cpp
  SearchServer.cpp
)

target_include_directories(engine_lib
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(engine_lib
  PUBLIC
    nlohmann_json::nlohmann_json
)

add_executable(search_engine_app main.cpp)

target_include_directories(search_engine_app PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(search_engine_app PRIVATE engine_lib)

set_target_properties(search_engine_app PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
