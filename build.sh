#!/bin/bash

# Script to build and run the C project using CMake
EXECUTABLE_NAME="assignment-opencl"
BUILD_DIR="build"
BIN_DIR="bin"

function display_help() {
    echo "Usage: $0 [OPTION] [--release or --debug]"
    echo "Options:"
    echo "  build           Build the project using CMake (main and tests)"
    echo "  run             Run the compiled binary"
    echo "  clean           Remove the build directory"
    echo "  all             Build and then run the project (build main and test, run only main)"
    echo "  test            Run tests"
    echo "  buildAndTest    Build and then test the project (build main and test, run only tests)"
    echo "  help            Display this help message"
}

# Ensure we have at least one argument
if [ "$#" -lt 1 ]; then
    echo "Error: No arguments provided."
    display_help
    exit 1
fi


BUILD_TYPE="Debug" # Default build type
if [[ "$2" == "--release" ]]; then
    BUILD_TYPE="Release"
elif [[ "$2" == "--debug" ]]; then
    BUILD_TYPE="Debug"
fi

case $1 in
    build)
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. &&
        make
        ;;

    run)
        ./"$BIN_DIR"/"$EXECUTABLE_NAME"
        ;;

    clean)
        rm -rf "$BUILD_DIR"/
        ;;

    all)
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. &&
        make && 
        cd ..
        ./"$BIN_DIR"/"$EXECUTABLE_NAME"
        ;;

    test)
        cd "$BUILD_DIR"
        ctest --verbose
        ;;

    buildAndTest)
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE .. &&
        make &&
        ctest --verbose
        ;;

    lint)
        if [ -d "$BUILD_DIR" ]; then
            cd "$BUILD_DIR" &&
            cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. &&
            run-clang-tidy
        else
            echo "No build directory exists."
        fi
        ;;

    help|*)
        display_help
        ;;
esac
