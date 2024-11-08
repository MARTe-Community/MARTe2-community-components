/**
 * @file Helpers.h
 * @brief helpers function and classes for easier validation of configurations
 * 
 * @details In this header are defined the basic tools to validate parameters and signals
 * of a MARTe component configuration.
 **/
#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "AnyType.h"
#include "StructuredDataI.h"

namespace EC {

/**
 * @brief False uint8 value
 **/
const MARTe::uint8 False = 0u;

/**
 * @brief True uint8 value
 **/
const MARTe::uint8 True = 1u;

/**
 * @brief Null/Void AnyType representation
 **/
const MARTe::AnyType Void = MARTe::voidAnyType;

/**
 * @brief A MARTe signal definition
 *
 * This structure can be used to define a Signal (optional or mandatory) of
 * a MARTe component. Using this structure will enable an automatic validation
 * of the configuration.
 **/
struct Signal {
  const MARTe::char8 *name;
  const MARTe::TypeDescriptor type;
  const bool optional;
  const MARTe::uint32 size;

  /**
   * @brief Signal constructor
   *
   * @param name of the signal (if empty string, any name can do it)
   * @param type of the signal
   * @param optional flag (false by default)
   * @param size of the signal (1 by default), useful for arrays
   **/
  Signal(const MARTe::char8 *name, MARTe::TypeDescriptor type,
         bool optional = false, MARTe::uint32 size = 1u);
};

/**
 * @brief Verify if the configuration of a signal match the definition
 *
 * This function can be used in the `initialise` method of a MARTe component
 * when iterating trought the signals in order to verify if the signal
 * configuration match the definition. The function expect the `StructuredDataI`
 * to be already in the correct signal.
 *
 * @param config of MARTe component at the correct position (child)
 * @param definition of the signal
 * @return `true` if the configuration matches or if the signal is optional
 * @return `false` otherwise
 **/
bool validate_signal(MARTe::StructuredDataI &config,
                     const EC::Signal definition);

/**
 * @brief Validate signals configuration for given list of definitions
 *
 * This function can be used in the `Initialise` to validate the `InputSignals`
 * or `OutputSignals` block with a given definition.
 *
 * @param configuration of the signals
 * @param definitions of the signals
 * @param n_signals number of signals defined
 * @param actual_number of signals verified (useful for optional signals)
 * @return true if all mandatory signalas are defined
 * @return false otherwise
 **/
bool validate_signals(MARTe::StructuredDataI &conf,
                      const EC::Signal *definitions,
                      const MARTe::uint32 n_signals,
                      MARTe::uint32 &actual_number);

/**
 * @brief Validate signals configuration for given list of definitions
 *
 * This is the same function as the full `validate_signals` but does not
 * take in account the actual number of signal verified
 *
 * @param configuration of the signals
 * @param definitions of the signals
 * @param n_signals number of signals defined
 * @return true if all mandatory signalas are defined
 * @return false otherwise
 **/
bool validate_signals(MARTe::StructuredDataI &conf,
                      const EC::Signal *definitions,
                      const MARTe::uint32 n_signals);

/**
 * @brief check that the number of signals is exactly the one desired.
 *
 * @param configuration to be verified
 * @param number of expected signals
 * @return true if the number of signals is exactly the one expected
 * @return false otherwise
 **/
bool enforce_exact_signals_number(MARTe::StructuredDataI &configuration,
                                  const MARTe::uint32 number);

/**
 * @brief check that the number of signals is greater or equal to the one
 * desired.
 *
 * @param configuration to be verified
 * @param min number of expected signals
 * @return true if the number of signals is greater or equal to the one expected
 * @return false otherwise
 **/
bool enforce_min_signals_number(MARTe::StructuredDataI &configuration,
                                const MARTe::uint32 min);

/**
 * @brief check that the number of signals is exactly the one desired.
 *
 * @param configuration to be verified
 * @param max number of expected signals
 * @return true if the number of signals is less or equal to the one expected
 * @return false otherwise
 **/
bool enforce_max_signals_number(MARTe::StructuredDataI &configuration,
                                const MARTe::uint32 max);

/**
 * @brief check that the number of signals is in a given range.
 *
 * @param configuration to be verified
 * @param min number of signals
 * @param max number of signals
 * @return true if the number of signals is in the expected range
 * @return false otherwise
 **/
bool enforce_range_signals_number(MARTe::StructuredDataI &configuration,
                                  const MARTe::uint32 min,
                                  const MARTe::uint32 max);
/**
 * @brief A filter function that verify a given MARTe::AnyType value
 *
 * This is used to define filters to validate parameters of MARTe components.
 * A few filters are defined below but any custom one can be implemented.
 **/
typedef bool (*Filter)(MARTe::AnyType);

/**
 * @brief namespace containinbg definitions of some default filters.
 **/
namespace filters {

/**
 * @brief a filter that match any value
 *
 * @param value to check
 * @return true
 **/
bool any(MARTe::AnyType value);

/**
 * @brief filter that verify if a parameter is in a given range.
 * @tparam T type of the value to check
 * @tparam min value of the valid range
 * @tparam max value of the valid range
 * @param x value to verify
 * @return true if min <= x <= max
 * @return false otherwise
 **/
template <typename T, T min, T max> inline bool limit(MARTe::AnyType x) {
  T v = *(T *)x.GetDataPointer();
  return v >= min && v <= max;
}
/**
 * @brief filter that verify if a parameter is greater or equal to a certain
 *value.
 * @tparam T type of the value to check
 * @tparam min valid value
 * @param x value to verify
 * @return true if min <= x
 * @return false otherwise
 **/
template <typename T, T min> inline bool from(MARTe::AnyType x) {
  T v = *(T *)x.GetDataPointer();
  return v >= min;
}
/**
 * @brief filter that verify if a parameter is minor or equal to a certain
 * value.
 * @tparam T type of the value to check
 * @tparam max valid value
 * @param x value to verify
 * @return true if x <= max
 * @return false otherwise
 **/
template <typename T, T max> inline bool to(MARTe::AnyType x) {
  T v = *(T *)x.GetDataPointer();
  return v <= max;
}
/**
 * @brief filter that verify if a uint8 is a valid boolean value (0 or 1)
 * @param x value to verify
 * @return true if x is 0 or 1
 * @return false otherwise
 **/
bool boolean(MARTe::AnyType a);
} // namespace filters

/**
 * @brief Parameter definition
 *
 * This structure can be used to define the expected parameter of a given
 *Component.
 **/
struct Parameter {
  const MARTe::char8 *name;          // name of the paremeter
  const MARTe::TypeDescriptor type;  // type of the parameter
  const Filter filter;               // filter to validate parameter value
  MARTe::AnyType defaultValue; // default value for the parameter

  /**
   * @brief Parameter constructor
   *
   * @param name of the parameter
   * @param type of the parameter
   * @param filter to valdiate the parameter (any function by default)
   * @param defaultValue if the parameter is not defined (Void by default)
   **/
  Parameter(const MARTe::char8 *name, const MARTe::TypeDescriptor type,
            const Filter filter = filters::any,
            const MARTe::AnyType &defaultValue = Void);
  ~Parameter();
};

/**
 * @brief validate the parameters of the configuration of a Component
 *
 * This function can be used to validate and set the parameters of a MARTe
 * component it will set the fields using the configured value or the default
 * and return true if all the mandatory parameters are defined.
 *
 * @param configuration of the component at its root position
 * @param definitions of the parameters
 * @param parameters in form of an array of AnyValue
 * @param n_parameters number of parameters
 * @return true if all mandatory parameters are configured correctly and if
 * all parameters pass the validation step
 * @return false otherwise
 **/
bool validate_parameters(MARTe::StructuredDataI &configuration,
                         const EC::Parameter *definitions,
                         MARTe::AnyType *parameters,
                         const MARTe::uint32 n_parameters);

/**
 * @brief verify that an element of the configuration has a given type
 * @param configuration of the element to verify
 * @param type required
 * @return true if the configuration match the type
 * @return false if `Type` field does not exist or does not match
 **/
bool verify_type(MARTe::StructuredDataI &configuration, const char *type);

}; // namespace EC

#endif
