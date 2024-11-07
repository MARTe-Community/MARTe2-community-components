# `Helpers.hpp`

This utility header file only code is used mostly to help the verication
of the configuration (in the `Initialise` and `Setup` methods).


## Data Types

### Signal
Used to define any input or output signals by specifing:
   - `const char8 * name`: name of the signal (if empty any name will be accepted)
   - `const TypeDescriptor type`: type of the signal
   - `const bool optional = false`: if the signal is optional or mandatory
   - `const uint32 size = 1u`: the dimension of the signal if it is an array

### Filter
Is a pointer to a function with the following signature `bool (MARTe::AnyType)` that is used to verify a parameter

### Parameter
Used to define any parameter by specifing:
   - `const char8 * name`: name of the parameter
   - `const TypeDescriptor type`: type of the parameter
   - `const Filter filter`: function to verify the value of the parameter
   - `const AnyType defaultValue`: default value if the parameter is not defined in the configuration

## Functions

### check_signal
This function can be used to check if the current signal of a `StructuredDataI` is the one defined.

#### Parameters
 - `StructuredDataI & config`: current MARTe configuration at the current signal definition
 - `const Signal def`: signal definition

#### Returns
Return `true` if the signal match the definition or if the definition is optional, `false` otherwise.

### validate_signals

### validate_parameters

### verify_type

### any

### from

### to

### limit

### boolean
