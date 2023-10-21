# Assignment OpenCL

Assignment OpenCL

## Table of Contents

- [Assignment OpenCL](#assignment-opencl)
- [Table of Contents](#table-of-contents)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
- [Testing](#testing)
- [Project Structure](#project-structure)

## Getting Started

These instructions will help you set up the project on your local machine for development and testing purposes.

### Prerequisites

Here's what you'll need to run the project:

- **GCC**: Preferred compiler.
  - Installation (based on your OS, e.g., sudo apt install gcc for Debian-based distros).
- **CMake**: Primary build tool.
   ```bash
   sudo apt-get update 
   sudo apt-get install cmake

### Installation

1. **Clone the Repository**
    ```bash
   git clone https://github.com/robertos99/assignment-opencl.git

2. **Navigate to the Project Directory**
    ```bash
   cd assignment-opencl

3. **Build the Project** (this will generate all the folders and download Unity dependancies)

    ```bash
    ./build.sh build

### Usage
After building, you can run the main application:
    
    ./build.sh run

or build the entire project and run the main application:

    ./build.sh all


### Testing
To run the tests, after building the project:    

    ./build.sh test

or build the entire project and run the tests:

    ./build.sh buildAndTest

### Project Structure
The directory structure of this project is as follows:

```
├── CMakeLists.txt
├── README.md
├── bin (generated, holds final binaries)
├── build (generated, holds relevant files for builds)
├── build.sh (centralized workflows)
├── src
│   ├── helloworld.c (example file for lib)
│   ├── helloworld.h
│   └── main.c (main)
├── tests
│   └── test_helloworld.c (example test for lib)
└── third_party (tools, etc.)