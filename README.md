# CANpie FD

The goal of this project is to define a standard Application Programming Interface (API) for access to the CAN bus.
The API provides functionality for ISO/OSI Layer-2 (Data Link Layer). It is not the intention of CANpie to incorporate
higher layer functionality (e.g. CANopen, J1939). The API covers support for **Classical CAN** frames as well as for
**ISO CAN FD** frames.

Where ever it is possible CANpie FD is independent from the used hardware and operating system. The function calls are
unique for different kinds of hardware. Also CANpie FD provides a method to gather information about the features of the
 CAN hardware. This is especially important for an application designer, who wants to write the code only once.

The API is designed for embedded control applications as well as for PC interface boards.

## Migration from CANpie to CANpie FD

The C language API of CANpie FD (i.e. version 3) is compatible to CANpie (i.e. version 2) with the exception that 8
functions are deprecated. Please refer to the section [migration](https://canpie.github.io/page_8api_migration.html)
of the documentation.

## Documentation

The documentation is available on <https://canpie.github.io/>.

## Configure and build

The source code for CANpie is platform independent and should compile successfully on any system or target.

### Prerequisites for build all projects and tests

All programs and tests can be compiled from the top-level CMake file. To do this, at least the following tools must be
available on your system.

* [CMake 3.16.4](https://cmake.org/)
* [Qt 5.9.1](http://download.qt.io/official_releases/qt/)
* C99 compiler
  * windows user can use for example MinGW compiler provided by [MSYS2](https://www.msys2.org)

#### FFF Module Setup

The Fake Function Framework is included to the test/fff as a Submodule, so perform following operations to clone FFF:

    git submodule init

and

    git submodule update

So the master of FFF is pulled directly to the specified folder.

### Build

To build all projects navigate to the root directory and perform the following operations:

    mkdir build
    cd build
    cmake ..
    cmake --build .

Alternatively you have the possibility to open the CANpie-FD directory from the
[Visual Studio code](https://code.visualstudio.com/) (VSC) and start the CMake.
Make sure that the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) Extension
for the VSC is installed.

Build Tools for Visual Studio 2017 (version 15.9)
For compilation of CANpie server and the corresponding plugins the build Tools has been downloaded from:
<https://my.visualstudio.com/Downloads?q=Visual%20Studio%202017>
