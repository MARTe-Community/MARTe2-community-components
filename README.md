# MARTe2 Community Components

## Components

### Datasources

- [`OnChangeLoggerDataSource`](Documentation/Datasources/OnChangeLoggerDataSource.md): is a modified logger that print only when the signals change

### GAMs

- [`LuaGAM`](Documentation/GAMs/LuaGAM.md): a GAM that can run a user defined LUA code

### Interfaces

- [`JSONLogger`](Documentation/Interfaces/JSONLogger.md): a logger that output `jsonl` files that can be used in combination of many standard tools
- [`HTTPDynamicMessageInterface`](Documentation/Interfaces/HttpDynamicMessageInterface.md): http dynamic message interface

## Core

### Utils

- [`Helpers.h`](Documentation/Core/Utils/Helpers.md): contains functions and types to help the definition of new components

### Types

- [`Types`](/Documentation/Core/Types/README.md): a set of advanced types used in different components

## Instruction

In order to compile and use the project you need the following dependencies:

- `MARTe2`

Before compiling the following environment variables are needed:

- `MARTe2_DIR`: root path of `MARTe2`
- `GTEST_DIR`: root path of `GTEST` library

To compile simply type `make -f Makefile.gcc all`
