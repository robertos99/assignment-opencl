cmake_minimum_required(VERSION 3.10)
project(assignment-opencl)

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
add_executable(assignment-opencl src/main.c)
target_link_libraries(assignment-opencl core_lib)
set_target_properties(assignment-opencl PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

## Unit tests using Unity
enable_testing()
add_executable(assignment-opencl_tests tests/test_helloworld.c ${unity_SOURCE_DIR}/src/unity.c)
target_link_libraries(assignment-opencl_tests core_lib)
target_include_directories(assignment-opencl_tests PRIVATE ${unity_SOURCE_DIR}/src)
set_target_properties(assignment-opencl_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_test(NAME assignment-opencl_tests COMMAND assignment-opencl_tests)
