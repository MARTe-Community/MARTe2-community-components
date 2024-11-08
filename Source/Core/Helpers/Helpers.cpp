#include "Helpers.h"
#include "AdvancedErrorManagement.h"
#include "AnyType.h"
#include "CompilerTypes.h"
#include "ErrorType.h"
#include "StreamString.h"
#include "StringHelper.h"
#include "TypeDescriptor.h"

namespace EC {

Signal::Signal(const MARTe::char8 *name, MARTe::TypeDescriptor type,
               bool optional, MARTe::uint32 size)
    : name(name), type(type), optional(optional), size(size) {}

Parameter::Parameter(const MARTe::char8 *name, const MARTe::TypeDescriptor type,
                     const Filter filter, const MARTe::AnyType &defVal)
    : name(name), type(type), filter(filter) {
  if (defVal.IsVoid()) {
    defaultValue = MARTe::AnyType();
  } else {
    char *mem = new char[defaultValue.GetByteSize()];
    memcpy(mem, defVal.GetDataPointer(), defVal.GetByteSize());
    defaultValue =
        MARTe::AnyType(defVal.GetTypeDescriptor(), defVal.GetBitAddress(), mem);
  }
}

Parameter::~Parameter() {
  if (!defaultValue.IsVoid()) {
    delete[] (char *)defaultValue.GetDataPointer();
  }
}

bool validate_signal(MARTe::StructuredDataI &config, const EC::Signal def) {
  MARTe::StreamString str;
  if (strlen(def.name) > 0) {
    if (strcmp(config.GetName(), def.name)) {
      fprintf(stderr, "Signal `%s` should have name `%s`", def.name,
              config.GetName());
      return false;
    }
  }
  if (!config.Read("Type", str) ||
      MARTe::TypeDescriptor::GetTypeDescriptorFromTypeName(str.Buffer()) !=
          def.type) {
    fprintf(stderr, "Signal `%s` should have type `%s`", def.name,
            MARTe::TypeDescriptor::GetTypeNameFromTypeDescriptor(def.type));
    return false;
  }
  if (def.size > 0) {
    // if size == 0 then any size will do
    MARTe::uint32 d = 1;
    MARTe::uint32 noe = 1;
    if (def.size != 1 && (!config.Read("NumberOfDimensions", d) ||
                          !config.Read("NumberOfElements", noe))) {

      fprintf(stderr,
              "Signal `%s` should have size `%d` but is missing "
              "part of defintion",
              def.name, def.size);
      return false;
    }
    if (d * noe != def.size) {
      fprintf(stderr,
              "Signal `%s` should have size %d but it has size %d"
              "part of defintion",
              def.name, def.size, d * noe);
      return false;
    }
  }
  return true;
}

bool validate_signals(MARTe::StructuredDataI &conf,
                      const EC::Signal *definitions,
                      const MARTe::uint32 n_signals,
                      MARTe::uint32 &actual_number) {
  bool ok = true;
  MARTe::uint32 i = 0, j = 0;

  MARTe::uint32 s_size = conf.GetNumberOfChildren();
  for (; i < n_signals && i < s_size; i++) {
    if (conf.MoveToChild(j)) {
      bool sig_chck = validate_signal(conf, definitions[i]);
      if (!sig_chck && !definitions[i].optional) {
        ok = false;

        REPORT_ERROR_STATIC(
            MARTe::ErrorManagement::InitialisationError,
            "Mandatory signal (%d) `%s` is missing or misstyped.", i,
            definitions[i].name);
      } else {
        j++;
      }
      ok = conf.MoveToAncestor(1u) & ok;
    } else {
      ok = false;
      REPORT_ERROR_STATIC(MARTe::ErrorManagement::InitialisationError,
                          "Something went wrong when moving to node %d of %s",
                          j, conf.GetName());
    }
  }
  for (; i < n_signals; i++) {
    if (!definitions[i].optional) {
      ok = false;
      REPORT_ERROR_STATIC(MARTe::ErrorManagement::InitialisationError,
                          "Missing mandatory signal `%s`", definitions[i].name);
    } else {
      REPORT_ERROR_STATIC(MARTe::ErrorManagement::Information,
                          "Missing optional signal %s", definitions[i].name);
    }
  }
  actual_number = j;
  return ok;
}

bool validate_signals(MARTe::StructuredDataI &conf,
                      const EC::Signal *definitions,
                      const MARTe::uint32 n_signals) {
  MARTe::uint32 _ = 0;
  return validate_signals(conf, definitions, n_signals, _);
}

bool enforce_exact_signals_number(MARTe::StructuredDataI &conf,
                                  const MARTe::uint32 num) {
  if (conf.GetNumberOfChildren() != num) {
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::InitialisationError,
                        "Exactly %d signals are needed for %s", num,
                        conf.GetName());
    return false;
  }
  return true;
}

bool enforce_min_signals_number(MARTe::StructuredDataI &conf,
                                const MARTe::uint32 num) {
  if (conf.GetNumberOfChildren() < num) {
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::InitialisationError,
                        "At least %d signals are needed for %s", num,
                        conf.GetName());
    return false;
  }
  return true;
}

bool enforce_max_signals_number(MARTe::StructuredDataI &conf,
                                const MARTe::uint32 num) {
  if (conf.GetNumberOfChildren() > num) {
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::InitialisationError,
                        "Maximum number of %d signals are allowed for %s", num,
                        conf.GetName());
    return false;
  }
  return true;
}

bool enforce_range_signals_number(MARTe::StructuredDataI &conf,
                                  const MARTe::uint32 min,
                                  const MARTe::uint32 max) {
  if (conf.GetNumberOfChildren() < min || conf.GetNumberOfChildren() > max) {
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::InitialisationError,
                        "Number ofsignals allowed for %s in from %d to %d",
                        conf.GetName(), min, max);
    return false;
  }
  return true;
}

bool filters::any(MARTe::AnyType) { return true; }

bool filters::boolean(MARTe::AnyType a) {
  if (a.GetTypeDescriptor() != MARTe::UnsignedInteger8Bit)
    return false;
  MARTe::uint8 v = *(MARTe::uint8 *)a.GetDataPointer();
  return v == 0u || v == 1u;
}

bool validate_parameters(MARTe::StructuredDataI &conf,
                         const EC::Parameter *definitions,
                         MARTe::AnyType *parameters,
                         const MARTe::uint32 n_params) {
  bool ok = true;
  for (MARTe::uint32 i = 0u; i < n_params && ok; i++) {
    if (!conf.Read(definitions[i].name, parameters[i])) {
      if (!definitions[i].defaultValue.IsVoid()) {
        memcpy(parameters[i].GetDataPointer(),
               definitions[i].defaultValue.GetDataPointer(),
               parameters[i].GetDataSize());
        REPORT_ERROR_STATIC(MARTe::ErrorManagement::Information,
                            "Using default value for parameter `%s` %!",
                            definitions[i].name, parameters[i]);
      } else {
        ok = false;
        REPORT_ERROR_STATIC(MARTe::ErrorManagement::InitialisationError,
                            "Missing mandatory paramter `%s`",
                            definitions[i].name);
      }
    } else {
      ok = definitions[i].filter(parameters[i]);
      if (!ok) {
        REPORT_ERROR_STATIC(
            MARTe::ErrorManagement::InitialisationError,
            "Parameter `%s` has value: `%!` and it is outside its limits.",
            definitions[i].name, parameters[i]);
      }
    }
  }
  return ok;
}

bool verify_type(MARTe::StructuredDataI &conf, const char *type) {
  MARTe::StreamString str;
  bool ok = true;
  if (!conf.Read("Type", str)) {
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::InitialisationError,
                        "Missing type information for signal named %s",
                        conf.GetName());
    ok = false;
  }
  if (ok && MARTe::StringHelper::Compare(type, str.Buffer())) {
    REPORT_ERROR_STATIC(
        MARTe::ErrorManagement::InitialisationError,
        "Signal named %s should be of type `%s` but is of type `%s`",
        conf.GetName(), type, str.Buffer());
    ok = false;
  }
  return ok;
}
} // namespace EC
