cmake_minimum_required(VERSION 3.10)
project(assignment-opencl)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set default compile flags for Debug and Release
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#=======================================
#==== Dependencies
#=======================================

include(FetchContent)

# Fetch Catch2
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.x
)
FetchContent_MakeAvailable(catch2)

find_package(OpenCV REQUIRED)

#=======================================
#==== Core-Lib
#=======================================

# Core library from source files
add_library(core_lib STATIC src/openCL-lib.cpp)
target_include_directories(core_lib PUBLIC src ${OpenCV_INCLUDE_DIRS})
target_link_libraries(core_lib OpenCL ${OpenCV_LIBS})
target_compile_definitions(core_lib PUBLIC CL_HPP_TARGET_OPENCL_VERSION=300)

# Apply general compile options
target_compile_options(core_lib PRIVATE -Wall -Wextra)

#=======================================
#==== Binaries
#=======================================

# Main executable
add_executable(assignment-opencl src/main.cpp)
target_link_libraries(assignment-opencl core_lib)
set_target_properties(assignment-opencl PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Apply general compile options
target_compile_options(assignment-opencl PRIVATE -Wall -Wextra)

#=======================================
#==== Testing Setup
#=======================================

# Enable testing with CTest
enable_testing()

# Unit tests using Catch2
add_executable(assignment-opencl_tests tests/test.cpp)
target_link_libraries(assignment-opencl_tests core_lib Catch2::Catch2)
set_target_properties(assignment-opencl_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(assignment-opencl_tests)