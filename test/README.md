# CANpie FD Testing

## The test/ folder

```project_folder
└── test/
    ├── canpie-fd/                 <- Unit tests for CANpie
    ├── fff/                       <- Fake Function Framework
    ├── fff_cpcore/                <- Fake functions for CANpie FD core 
    ├── test-canpie-core/          <- Unit tests for CANpie FD
    └── test-canpie-nrl/           <- FFF based test cases for NRL 
```

This folder contains test cases for the software from the source/ directory. In addition to the test cases themselves, 
the test frameworks such as FFF or unity are stored here. FFF is a open source micro-framework for creating fake C 
functions for tests. For additional information to FFF framework: [FFF documentation](test/fff/README.md).

## Compile and run tests

The most test cases can be compiled by the system compiler independent of OS using CMake.

### Requirements

* minimum required CMake version 3.16.3
* system compiler
* Fake Function Framework

#### FFF Module Setup

The Fake Function Framework is included to the test/fff as a Submodule, so perform following operations to clone FFF:

    git submodule init

and

    git submodule update

So the master of FFF is pulled directly to the specified folder.

### Compilation

To compile and run tests navigate to project folder and enter the following commands:

  ``` shell
  > mkdir build
  > cd build
  > cmake ..
  > cmake --build .
  > ctest 
  ``` 
Alternatively you have the possibility to open the CANpie-FD directory from the 
[Visual Studio code](https://code.visualstudio.com/) (VSC) and start the CMake.
Make sure that the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) Extension
for the VSC is installed.
