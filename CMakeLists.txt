cmake_minimum_required(VERSION 3.10)
project(assignment-opencl)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#=======================================
#==== Dependencies
#=======================================

include(FetchContent)

# Fetch Catch2
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.x  # Use the desired tag or version
)
FetchContent_MakeAvailable(catch2)

# Fetch stb
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

#=======================================
#==== Core-Lib
#=======================================

## Core library from source files
add_library(core_lib STATIC src/helloworld.cpp src/stb_image.cpp)
target_include_directories(core_lib PUBLIC src ${stb_SOURCE_DIR})
target_link_libraries(core_lib OpenCL)
target_compile_definitions(core_lib PUBLIC CL_TARGET_OPENCL_VERSION=100)
target_compile_options(core_lib PRIVATE -Wall -Wextra)

#=======================================
#==== Binaries
#=======================================

## Main executable
add_executable(assignment-opencl src/main.cpp)
target_link_libraries(assignment-opencl core_lib)
set_target_properties(assignment-opencl PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

## Main Debug executable
add_executable(assignment-opencl_debug src/main.cpp)
target_link_libraries(assignment-opencl_debug core_lib)
target_compile_options(assignment-opencl_debug PRIVATE "-g")
set_target_properties(assignment-opencl_debug PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


# Test setup
## Unit tests using Catch2
enable_testing()
add_executable(assignment-opencl_tests tests/test_helloworld.cpp)
target_link_libraries(assignment-opencl_tests core_lib Catch2::Catch2)
set_target_properties(assignment-opencl_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

include(CTest)
include(Catch)
catch_discover_tests(assignment-opencl_tests)
