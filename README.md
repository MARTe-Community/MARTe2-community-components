# MARTe2 Community Components

## Components

### Datasources

- [`OnChangeLoggerDataSource`](Documentation/Datasources/OnChangeLoggerDataSource.md): is a modified logger that print only when the signals change

### GAMs

- [`LuaGAM`](Documentation/GAMs/LuaGAM.md): a GAM that can run a user defined LUA code

### Interfaces

- [`JSONLogger`](Documentation/Interfaces/JSONLogger.md): a logger that output `jsonl` files that can be used in combination of many standard tools
- [`HTTPDynamicMessageInterface`](Documentation/Interfaces/HttpDynamicMessageInterface.md): http dynamic message interface

### Utils

- [`Helpers.h`](Documentation/Utils/Helpers.md): contains functions and types to help the definition of new components

## Instruction

In order to compile and use the project you need the following dependencies:

- `MARTe2`

Before compiling the following environment variables are needed:

- `MARTe2_DIR`: root path of `MARTe2`
- `GTEST_DIR`: root path of `GTEST` library

To compile simply type `make -f Makefile.gcc all`

You can use the `Startup/Main.sh` script to execute a MARTe app
please ensure that `MARTe2-components` is also installed in order to be
properly able to load a complete MARTe configuration.
