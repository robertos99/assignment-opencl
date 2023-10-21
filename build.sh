#!/bin/bash

# Script to build and run the C project using CMake
EXECUTABLE_NAME="helloworld"
BUILD_DIR="build"
BIN_DIR="bin"

# Function to display help/usage
function display_help() {
    echo "Usage: $0 [OPTION]"
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

# Handle the provided argument
case $1 in
    build)
        mkdir -p $BUILD_DIR
        cd $BUILD_DIR
        cmake ..
        make
        ;;

    run)
        ./$BIN_DIR/$EXECUTABLE_NAME
        ;;

    clean)
        rm -rf $BUILD_DIR/
        ;;

    all)
        mkdir -p $BUILD_DIR
        cd $BUILD_DIR
        cmake ..
        make
        cd ..
        ./$BIN_DIR/$EXECUTABLE_NAME
        ;;

    test)
        cd $BUILD_DIR
        ctest --verbose
        ;;

    buildAndTest)
        cd $BUILD_DIR
        cmake ..
        make
        ctest --verbose
        ;;

    help|*)
        display_help
        ;;
esac
