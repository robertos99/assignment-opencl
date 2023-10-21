cmake_minimum_required(VERSION 3.10)
project(helloworld)

## Dependencies
include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG master  # or a specific version or commit
)
FetchContent_GetProperties(unity)
if(NOT unity_POPULATED)
    FetchContent_Populate(unity)
    include_directories(${unity_SOURCE_DIR}/src)
endif()
FetchContent_MakeAvailable(unity)


## Core library from source files
add_library(core_lib STATIC src/helloworld.c)
target_include_directories(core_lib PUBLIC src)

## Main executable
add_executable(helloworld src/main.c)
target_link_libraries(helloworld core_lib)
set_target_properties(helloworld PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

## Unit tests using Unity
enable_testing()
add_executable(helloworld_tests tests/test_helloworld.c ${unity_SOURCE_DIR}/src/unity.c)
target_link_libraries(helloworld_tests core_lib)
target_include_directories(helloworld_tests PRIVATE ${unity_SOURCE_DIR}/src)
set_target_properties(helloworld_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_test(NAME helloworld_tests COMMAND helloworld_tests)
