# LuaGAM

This `GAM` executes user-defined Lua code.
The code is parsed by a dedicated parser, which checks that the `GAM` signals are correctly mapped and used into the Lua code.


## Configuration

The configuration of the `GAM` has the following mandatory global parameter:

| Field  | Type  | Description                                                                                                                       |
|:------:|:-----:|:----------------------------------------------------------------------------------------------------------------------------------|
| `Code` | char8 | Lua code to be executed. If the parameter is in the format `file://$PATH` the Lua code is loaded from the file located in `PATH`. |



The configuration of the `GAM` has the following optional additional fields:

| Field                | Description                                                                                                        |
|:--------------------:|:-------------------------------------------------------------------------------------------------------------------|
| `InternalStates`     | Field in which the non-GAM-signal global variables are initialized.                                                |
| `AuxiliaryFunctions` | Field in which auxiliary functions to be used in the code are declared. The usage in the main code is not checked. |

The user can define an arbitrary number of `InputSignals` and `OutputSignals` of any number type.


### Lua code

The parameter `Code` must contain only the global function `GAM()` without input parameters, in which the GAM logic is declared.

Everything outside the main function leads to a parsing error.

Every external part of code must be declared inside `InternalStates` or `AuxiliaryFunctions`.


### Example

An example of MARTe configuration:

```json
{
  "+LuaFunction": {
      "Class": "LuaGAM",
      "Code": "function GAM() if (InSig > fun(Thresh+offset)) OutSig = true else OutSig = false end end",
      "InternalStates": {
        "offset": "1.3"
      },
      "AuxiliaryFunctions": {
        "fun": "function fun(x) 3.4*x*sqrt(x) end"
      },
      "InputSignals": {
          "InSig": {
              "DataSource": "DDBinput",
              "Type": "float32"
          },
          "Thresh": {
              "DataSource": "DDBinput",
              "Type": "float32"
          }
      },
      "OutputSignals": {
          "OutSig": {
              "DataSource": "DDBoutput",
              "Type": "bool"
          }
      }
  }
}
```

## Functionality

This `GAM` allows to directly write the GAM logic in the MARTe configurations file through the Lua code.

At every execution, the input signals are pushed into the Lua stack. The code is then executed and then
the output signals are retrieved from the stack.

Below a simplification of the execution cycle of this `GAM`:

![Activity diagram of GAM execute](http://www.plantuml.com/plantuml/png/bP9DYzim48RlyojUzcH3MxPNXy267dh9Gbcsz18eYZrsXgWZ8uqA-VULv9OszBJa4jxvc4SFVahcekKVdjeV6i8LUrZClAFXXTq4qJLRsf46OqHPBaRu5DORxC9uFhoyCZuVdhSs95dK-GJ2J-Sp9rXeVSUlXv94SMVFA9V6AmWvYIuOdVVd4YICaPtn16SG5HFd9JcJe1z0h1Cg0T7Wi_k-i2q-d9tlZk_mKEu_z3WfE4RiKHlMihOM7-klt-4E__KY2P8QsHWcXZ9FF5NOEjRoYiSH4y-YFBK7R1VRQ-UiOoFx2oOpkdP_ZyU-hb1qDquz0SEtACO8sJQLQ1FmF1i1pUP_bj8_1WcujrrNtUl6csm2Vx11x1EZqz10mtK_knASzBTb2sjtx4_w9OfQzHlYLbjBuVt7QhQiafc6o9QZ8bntbCeKFPMyV5U_0000)
