# Transport Catalogue
This is an implementation of the transport directory, which provides work with JSON requests.
The route rendering request is answered with an SVG format string. The JSON constructor is implemented using a chain of method calls, obvious errors are at the compilation stage.
___
<!--A block of information about the repository in badges-->
![Static Badge](https://img.shields.io/badge/Nikolai_Dev-Transport_Catalogue-blue?style=plastic) ![GitHub License](https://img.shields.io/github/license/nikolai-gromov/cpp-transport-catalogue?style=plastic) ![GitHub top language](https://img.shields.io/github/languages/top/nikolai-gromov/cpp-transport-catalogue?style=plastic) ![GitHub language count](https://img.shields.io/github/languages/count/nikolai-gromov/cpp-transport-catalogue?style=plastic) ![GitHub repo stars](https://img.shields.io/github/stars/nikolai-gromov/cpp-transport-catalogue) ![GitHub issues](https://img.shields.io/github/issues/nikolai-gromov/cpp-transport-catalogue?style=plastic)

[![Logotype](/docs/logo.jpg)]()
___


The development was carried out in the Windows Subsystem for Linux (WSL) from Visual Studio Code.
<!--Setting-->
## Setting up the development environment

* [Using C++ and WSL in VS Code](https://code.visualstudio.com/docs/cpp/config-wsl)
* [Get started with CMake Tools on Linux](https://code.visualstudio.com/docs/cpp/cmake-linux)
* Installing Protobuf ([protobuf-cpp-3.21.12.tar.gz](https://github.com/protocolbuffers/protobuf/releases/tag/v21.12))
    * Unzip the Google Protocol Buffers archive
    ```
    tar -xzf protobuf-cpp-3.21.12.tar.gz
    ```
    * In the directory, run the following commands
    ```
    cmake <path to unpacked archive>/protobuf-3.21.12  \
      -DCMAKE_BUILD_TYPE=Debug                         \
      -Dprotobuf_BUILD_TESTS=OFF                       \
      -DCMAKE_INSTALL_PREFIX=<path to package install location>/protobuf
    cmake --build .
    cmake --install .
    ```
    Static libraries built in the Debug configuration have the suffix ```d```, there will be no conflict.
    ```
    cmake <path to unpacked archive>/protobuf-3.21.12  \
      -DCMAKE_BUILD_TYPE=Release                       \
      -Dprotobuf_BUILD_TESTS=OFF                       \
      -DCMAKE_INSTALL_PREFIX=<path to package install location>/protobuf
    cmake --build .
    cmake --install .
    ```
    * The directory structure should look like this
    ```
    protobuf/
    ├── bin/
    │   └── protoc (protoc.exe) /*proto file compiler*/
    ├── include/google/protobuf/
    │   └── include-files
    └── lib/
        ├── pkgconfig/ /*description of library for pkg-config program, which automatically configures assembly of C++ libraries*/
        ├── lib*.a (lib*.lib under Visual Studio) /*static libraries*/
        └── ...
    ```
    * Now you can use the installed Google Protocol Buffers libraries by specifying the paths to them in the build settings of your project
    ```
    -I/path/to/your/project/protobuf/include -L/path/to/your/project/protobuf/lib -lprotobuf
    ```