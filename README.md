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
- [Tooling](#tooling)

## Getting Started

These instructions will help you set up the project on your local machine for development and testing purposes.

### Prerequisites

Here's what you'll need to run the project:

- **GCC/G++**: Preferred compiler.
  - Installation (based on your OS, e.g., sudo apt install gcc g++ for Debian-based distros).
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
│   ├── lib.cpp (example file for lib)
│   ├── lib.hpp
│   └── main.cpp (main)
├── tests
│   └── test_lib.cpp (example test for lib)
└── third_party (tools, etc.)
```
### Tooling

#### Linting

This project uses clang-tidy for linting. We use the LLVM ```.clang-tidy``` rules.

To run the linter on the whole project run: 
```bash
./build.sh lint
```

The project is configured to work with both ```Clangd``` and ```C/C++-Extension``` for VSCode.

#### Formatting

This project uses clang-format for linting. We use the LLVM ```.clang-format``` rules.
