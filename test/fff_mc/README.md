# FFF usage at MicroControl

For the use of FFF at MicroControl, the `fff_mc.h` is to be used in the test cases.

The requirement was to align this framework with the Unity API. Which is why the corresponding macros
have been created.

If required, further `ASSERT_` macros can be defined here.

## Requirements

It is necessary that the FFF module is available. This is included in the project as a Git submodule.

For the setup, follow the steps described in [FFF Module Setup](./../../README.md/#fff-module-setup).

## Directory structure and their names

With the realised directory structure, it is possible to integrate the test cases in different, higher-level projects without collisions or conflicts arising.

Here is a list of the directory structure, which can apply to several projects.

```text
<test>/                             <- test folder
├── fff/                            <- Fake Function Framework (fff)
├── fff_mc/                           
│   └── README.md                   <- documentation you are reading right now
├── fff_cpcore/                     <- fakes of CANpie Core for reuse across multiple test-suites
├── fff_cos/                        <- fakes of CANopen Slave for reuse across multiple test-suites
├── fff_com/                        <- fakes of CANopen Master for reuse across multiple test-suites
├── fff_mcl/                        <- fakes of MicroControl Library for reuse across multiple test-suites
├── fff_cpcore_tests/               <- contains collection of tests for CANpie Core 
├── fff_com_tests/                  <- contains collection of tests for CANopen Master
├── fff_cos_tests/                  <- contains collection of tests for CANopen Slave
│   ├── test-cos-301/               <- test implementation of CiA 301 sub module
│   ├── test-cos-emcy/              <- test implementation of emergency sub module
│   ├── test-profile-401/           <- test implementation of CiA 401 sub module
│   └── .../                        <- further test implementations
├── fff_app/                        <- contains collection of tests for application
│   ├── test-app-mod1/              <- test implementation of application module 1
│   └── .../                        <- further test implementations
└── .../                            <- further collections of tests
```

The names of the directories were chosen in such a way that it is obvious whether the content is the implementation of test cases or the fakes.

It must be noted that the fakes are used by several test cases. This is how it is described here: [How do I reuse a fake across multiple test-suites?](./../fff/README.md/#how-do-i-reuse-a-fake-across-multiple-test-suites).

## Files and their names

A difference is made between two implementations of the files:

* implementation of fakes with `ff_` prefix
* implementation of test cases with `test_` prefix

Here is a list of the file names within the directory structure defined above.

```text
<test>/                             <- test folder
├── fff_cpcore/                     <- fakes of CANpie Core for reuse across multiple test-suites
│   ├── ff_cp_core.c                <- definition of fakes
│   ├── ff_cp_core.h                <- declaration of fakes
│   └── .../
├── fff_cos_tests/                  <- contains collection of tests for CANopen Slave
│   ├── test-profile-401/           <- test implementation of CiA 401 sub module
│   │   ├── CMakeLists.txt          <- CMake configuration for the desired test
│   │   ├── test_cos401_main.c      <- test suite with main() 
│   │   ├── test_cos401di.c         <- test cases for CiA 401 DI
│   │   ├── test_cos401do.c         <- test cases for CiA 401 DO
│   │   └── ...
│   └── .../
└── .../
```

## Build and execute the tests

CMake build system is used to build the tests. For this reason, each directory contains a CMakeLists.txt,
which allows us to build either selected tests or all tests.
See [Build](./../../README.md/#build) for further information, how to build the project.

If a test was successfully built, the corresponding program can be executed.
The complete output of FFF is displayed in the console together with the debug output.

```shell
#-------------------------------------------------------------------#
# Unit Testing of MCL CANpie FD modules based on FFF
#-------------------------------------------------------------------#

Run CP FIFO Test with FUNCTION implementation!

--- Resetting test suite CP_FIFO: 
--- Running test suite CP_FIFO:
    >>> Running CP_FIFO.Initialization:
    >>> Running CP_FIFO.WriteAndRead:
...

#-------------------------------------------------------------------#
# --- FINISH ---                                                    #
#-------------------------------------------------------------------#

```

Alternatively, the CTest can be run:

```shell
[ctest]     Start 1: test-canpiefd
[ctest] 1/1 Test #1: test-canpiefd ....................   Passed    0.01 sec
[ctest] 
[ctest] 100% tests passed, 0 tests failed out of 1
[ctest] 
[ctest] Total Test time (real) =   0.02 sec
[ctest] CTest finished with return code 0
```

The first approach is used more during development.
The second approach is very helpful when many tests are executed in nested projects and only the result is to be output.
