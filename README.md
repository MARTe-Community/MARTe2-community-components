# EC Components

EC specific MARTe components.

## Components

### Datasources
 - [`OnChangeLoggerDataSource`](Documentation/Datasources/OnChangeLoggerDataSource.md): is a modified logger that print only when the signals change
 - [`VInstDataSource`](Documentation/VInst/VInstDataSource.md): input data source that can read signals coming from a `VInstDevice`

### GAMs
 - [`AnalogProtectionGAM`](Documentation/GAMs/AnalogProtectionGAM.md): a GAM to detect faults in analog signals
 - [`CommandMapperGAM`](Documentation/GAMs/CommandMapperGAM.md): a GAM to map different signals and values to a single command output (used to control the real-time state machine)
 - [`ComparatorGAM`](Documentation/GAMs/ComparatorGAM.md): a GAM to compare any number of pairs of signals to ensure that each pair is exactly the same if not a flag is raisen 
 - [`JADA::RTStateMachineGAM`](Documentation/GAMs/JADA/RTStateMachineGAM.md): the real-time state machine for the JADA gyrotron SCU
 - [`LuaGAM`](Documentation/GAMs/LuaGAM.md): a GAM that can run a user defined LUA code
 - [`MonitorGAM`](Documentation/GAMs/MonitorGAM.md): a GAM to monitor signals to check that are alive (changing) and if not flag it
 - [`SimSequenceGAM`](Documentation/GAMs/SimSequenceGAM.md): a GAM to simulate the configuration sequence of a subsystem (could be replaced with a script GAM)
 - [`TriangularReferencesGAM`](Documentation/GAMs/TriangualrReferencesGAM.md): a GAM to generate the time and amplitude references used to generate a triangular waveform using the [[WaveformGenGAM]] (could be replaced with a script GAM)
 - [`VInstSequenceGAM`](Documentation/VInst/VInstSequenceGAM.md): a GAM to manage configuration and command sequence of `VInstDevice`
 - [`WaveformGenGAM`](Documentation/GAMs/WaveformGenGAM.md): a GAM to generate any custom waveform by defining reference timing and amplitude arrays

### Interfaces
 - [`JSONLogger`](Documentation/Interfaces/JSONLogger.md): a logger that output `jsonl` files that can be used in combination of many standard tools
 - [`VInstDevice`](Documentation/VInst/VInstDevice.md): interface to communicate to auxiliairies device using different protocols and communication layers (`TCP`, `TelNet`, `UART`) abstracted by the `vinstlib` (previously `scpilib`):
   - [`SCPIDevice`](Documentation/VInst/VInstDevice.md#scpidevice): implementation of the `SCPI` protocol
   - [`JASTECDevice`](Documentation/VInst/VInstDevice.md#jastecdevice): implementation of the `JASTEC` custom protocol 

### Utils
 - [`Helpers.h`](Documentation/Utils/Helpers.md): contains functions and types to help the definition of new components

## Instruction

In order to compile and use the project you need the following dependencies:
 - `MARTe2`
 - `ec-types` base types and tools from the EC section
 - `SCPILib` other project from the EC section
 - `EC-TYPES` other project from the EC section

Before compiling the following environment variables are needed:
 - `MARTe2_DIR`: root path of `MARTe2`
 - `SCPI_DIR`: root path of `SCPILib`
 - `EC_ROOT` : root path of EC project  
 - `EC_TYPES`: root path of `EC-TYPES`
 - `GTEST_DIR`: root path of `GTEST` library 

To compile simply type `make -f Makefile.gcc all`

You can use the `Startup/Main.sh` script to execute a MARTe app
please ensure that `MARTe2-components` is also installed in order to be
properly able to load a complete MARTe configuration.
