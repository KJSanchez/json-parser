cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-g -Wall -fmodules-ts)

project(cpp CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_SCAN_FOR_MODULES YES)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.8
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(test test.cpp)
add_library(json STATIC json.cpp)

target_include_directories(json PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(json PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(test PRIVATE Catch2::Catch2 nlohmann_json::nlohmann_json)
