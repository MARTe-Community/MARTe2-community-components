/**
 * @file LuaGAM.cpp
 * @brief Source file for the LuaGAM class
 * @date 17/01/2024
 * @author Andrea Antonione
 * @author Martino Ferrari
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing,
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class LuaGAM (public, protected, and private). Be aware
 * that some methods, such as those inline could be defined on the header file,
 * instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "LuaGAM.h"
#include "AdvancedErrorManagement.h"
#include "CompilerTypes.h"
#include "DataSourceI.h"
#include "ErrorType.h"
#include "Helpers.h"
#include "LuaParser.h"
#include "StreamString.h"
#include "StringHelper.h"
#include "StructuredDataI.h"
#include "TypeDescriptor.h"
#include "Verifier.h"
#include "Option.h"

#include <cstring>
#include <stdint.h>

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

#define ERROR_GET_TABLE(name)                                                  \
  lua_pop(L, -1);                                                              \
  REPORT_ERROR_STATIC(ErrorManagement::IllegalOperation,                       \
                      "Impossible to assign value to signal, the Lua "         \
                      "variable `%s` is not a table.",                         \
                      name);
#define ERROR_GET_NUMBER(name)                                                 \
  lua_pop(L, -1);                                                              \
  REPORT_ERROR_STATIC(ErrorManagement::IllegalOperation,                       \
                      "Impossible to assign value to signal, the Lua "         \
                      "variable `%s` is not a number.",                        \
                      name);
#define ERROR_GET_NUMBER_ELEMENT(name, idx)                                    \
  REPORT_ERROR_STATIC(ErrorManagement::IllegalOperation,                       \
                      "Impossible to assign value to signal, the element %i "  \
                      "of the Lua table `%s` is not a number.",                \
                      idx, name);
#define ERROR_LUA_VAR_RANGE(name)                                              \
  lua_pop(L, -1);                                                              \
  lua_pop(L, -1);                                                              \
  REPORT_ERROR_STATIC(ErrorManagement::IllegalOperation,                       \
                      "Impossible to assign value to signal, the Lua "         \
                      "variable `%s` is out of range.",                        \
                      name);
#define ERROR_LUA_TAB_RANGE(name, idx)                                         \
  lua_pop(L, -1);                                                              \
  lua_pop(L, -1);                                                              \
  REPORT_ERROR_STATIC(ErrorManagement::IllegalOperation,                       \
                      "Impossible to assign value to signal, the element %i "  \
                      "of the Lua table `%s` is out of range.",                \
                      idx, name);
#define ERROR_SIG_VAR_RANGE(name)                                              \
  REPORT_ERROR_STATIC(ErrorManagement::IllegalOperation,                       \
                      "Impossible to assign value to Lua variable, the "       \
                      "signal `%s` is out of range.",                          \
                      name);
#define ERROR_SIG_ARR_RANGE(name, idx)                                         \
  REPORT_ERROR_STATIC(ErrorManagement::IllegalOperation,                       \
                      "Impossible to assign value to Lua variable, the "       \
                      "element %i of the signal `%s` is out of range.",        \
                      idx, name);
#define REPORT_ERROR_OVERFLOW(name, type)                                      \
  REPORT_ERROR(ErrorManagement::Warning,                                       \
               "Signal %s has type %s. Potential Lua variable overflow.",      \
               name, type);

#define DEBUG_LOG(...)                                                         \
  printf("DEBUG_LOG [%s:%d]\t", __FILE__, __LINE__);                           \
  printf(__VA_ARGS__);

namespace MARTe {

static int wrap_exceptions(lua_State *L, lua_CFunction f) {
  try {
    return f(L);
  } catch (const char8 *s) {
    lua_pushstring(L, s);
  } catch (std::exception &e) {
    lua_pushstring(L, e.what());
  } catch (...) {
    lua_pushliteral(L, "caught (...)");
  }
  return lua_error(L); // Rethrow as a Lua error.
}

static int lua_utime(lua_State *L) {
  lua_pushnumber(L, MARTe::HighResolutionTimer::Counter() / 1000.0);
  return 1;
}

static int lua_mtime(lua_State *L) {
  lua_pushnumber(L, MARTe::HighResolutionTimer::Counter() / 1000000.0);
  return 1;
}

static int lua_log(lua_State *L) {
  int n = lua_gettop(L);
  if (n != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "marte.log needs an error code and a string\n");
    lua_error(L);
  } else {
    const char *msg = lua_tostring(L, 1);
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::Information, msg);
  }
  return 0;
}

static int lua_log_debug(lua_State *L) {
  int n = lua_gettop(L);
  if (n != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "marte.debug needs an error code and a string\n");
    lua_error(L);
  } else {
    const char *msg = lua_tostring(L, 1);
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::Debug, msg);
  }
  return 0;
}

static int lua_log_error(lua_State *L) {
  int n = lua_gettop(L);
  if (n != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "marte.error needs an error code and a string\n");
    lua_error(L);
  } else {
    const char *msg = lua_tostring(L, 1);
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::RecoverableError, msg);
  }
  return 0;
}

static int lua_log_warning(lua_State *L) {
  int n = lua_gettop(L);
  if (n != 1 || !lua_isstring(L, 1)) {
    lua_pushstring(L, "marte.warning needs an error code and a string\n");
    lua_error(L);
  } else {
    const char *msg = lua_tostring(L, 1);
    REPORT_ERROR_STATIC(MARTe::ErrorManagement::Warning, msg);
  }
  return 0;
}

static const luaL_Reg marte_fns[] = {
    {"utime", &lua_utime},
    {"mtime", &lua_mtime},
    {"log", &lua_log},
    {"debug", &lua_log_debug},
    {"warning", &lua_log_warning},
    {"error", &lua_log_error},
};

static bool init(lua_State *L) {
  lua_pushlightuserdata(L, (void *)wrap_exceptions);
  bool ok = luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
  lua_pop(L, 1);
  luaL_openlibs(L);
  luaL_register(L, "marte", marte_fns);
  return ok;
}

/**
 * @brief Push onto the Lua stack a float signal and assign its name to the Lua
 * variable
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the Lua variable
 * @param[in] ptr pointer to the GAM input signal
 * @param[in] size default to 1 for non-array variables
 */
template <typename T>
bool push_float(lua_State *L, const char8 *name, void *ptr, uint32 size) {
  lua_pushnumber(L, *(T *)ptr);
  lua_setglobal(L, name);
  return true;
}

/**
 * @brief Push onto the Lua stack a float array signal and assign its name to
 * the Lua table
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the Lua variable
 * @param[in] ptr pointer to the GAM input signal
 * @param[in] size array size
 */
template <typename T>
bool push_float_array(lua_State *L, const char8 *name, void *ptr, uint32 size) {
  T *array = (T *)ptr;
  lua_newtable(L);
  for (uint32 i = 0; i < size; i++) {
    lua_pushinteger(L, i + 1);
    lua_pushnumber(L, array[i]);
    lua_settable(L, -3);
  }
  lua_setglobal(L, name);
  return true;
}

/**
 * @brief Push onto the Lua stack an integer signal and assign its name to the
 * Lua variable
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the Lua variable
 * @param[in] ptr pointer to the GAM input signal
 * @param[in] size default to 1 for non-array variables
 */
template <typename T>
bool push_integer(lua_State *L, const char8 *name, void *ptr, uint32 size) {
  T var = *(T *)ptr;
  if ((var - LUA_MAX_INTEGER) > 0 || (-var - LUA_MAX_INTEGER) > 0) {
    ERROR_SIG_VAR_RANGE(name);
    return false;
  }
  lua_pushinteger(L, var);
  lua_setglobal(L, name);
  return true;
}

/**
 * @brief Push onto the Lua stack an integer array signal and assign its name to
 * the Lua table
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the Lua variable
 * @param[in] ptr pointer to the GAM input signal
 * @param[in] size array size
 */
template <typename T>
bool push_integer_array(lua_State *L, const char8 *name, void *ptr,
                        uint32 size) {
  T *array = (T *)ptr;
  lua_newtable(L);
  for (uint32 i = 0; i < size; i++) {
    if ((array[i] - LUA_MAX_INTEGER) > 0 || (-array[i] - LUA_MAX_INTEGER) > 0) {
      ERROR_SIG_ARR_RANGE(name, i);
      return false;
    }
    lua_pushinteger(L, i + 1);
    lua_pushinteger(L, array[i]);
    lua_settable(L, -3);
  }
  lua_setglobal(L, name);
  return true;
}

/**
 * @brief Push onto the Lua stack a boolean signal and assign its name to the
 * Lua variable
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the Lua variable
 * @param[in] ptr pointer to the GAM input signal
 * @param[in] size default to 1 for non-array variables
 */
bool push_boolean(lua_State *L, const char8 *name, void *ptr, uint32 size) {
  uint8 var = *(uint8 *)ptr;
  if (var > 1) {
    ERROR_SIG_VAR_RANGE(name);
    return false;
  }
  lua_pushboolean(L, var == 1);
  lua_setglobal(L, name);
  return true;
}

/**
 * @brief Push onto the Lua stack a boolean array signal and assign its name to
 * the Lua table
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the Lua variable
 * @param[in] ptr pointer to the GAM input signal
 * @param[in] size array size
 */
bool push_boolean_array(lua_State *L, const char8 *name, void *ptr,
                        uint32 size) {
  uint8 *array = (uint8 *)ptr;
  lua_newtable(L);
  for (uint32 i = 0; i < size; i++) {
    if (array[i] > 1) {
      ERROR_SIG_ARR_RANGE(name, i);
      return false;
    }
    lua_pushinteger(L, i + 1);
    lua_pushboolean(L, array[i]);
    lua_settable(L, -3);
  }
  lua_setglobal(L, name);
  return true;
}

/**
 * @brief Extract from the Lua stack the float variable identified by its name
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the variable
 * @param[in] ptr pointer to the GAM output signal
 * @param[in] size default to 1 for non-array variables
 * @return true if the variable is a number
 */
template <typename T>
bool get_float(lua_State *L, const char8 *name, void *ptr, uint32 size) {
  lua_getglobal(L, name);
  int is_num = 0;
  *(T *)ptr = (T)lua_tonumberx(L, -1, &is_num);
  if (!is_num) {
    ERROR_GET_NUMBER(name);
    return false;
  }
  lua_pop(L, -1);
  return true;
}

/**
 * @brief Extract from the Lua stack the float array (Lua table) identified by
 * its name
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the variable
 * @param[in] ptr pointer to the GAM output signal
 * @param[in] size array size
 * @return true if the variable is an array (Lua table) and each element is a
 * number
 */
template <typename T>
bool get_float_array(lua_State *L, const char8 *name, void *ptr, uint32 size) {
  lua_getglobal(L, name);
  if (!lua_istable(L, -1)) {
    ERROR_GET_TABLE(name);
    return false;
  }
  T *array = (T *)ptr;
  for (uint32 i = 0; i < size; i++) {
    lua_rawgeti(L, -1, i + 1);
    int is_num = 0;
    array[i] = (T)lua_tonumberx(L, -1, &is_num);
    if (!is_num) {
      ERROR_GET_NUMBER_ELEMENT(i, name);
      return false;
    }
    lua_pop(L, -2);
  }
  lua_pop(L, -1);
  return true;
}

/**
 * @brief Extract from the Lua stack the signed integer variable identified by
 * its name
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the variable
 * @param[in] ptr pointer to the GAM output signal
 * @param[in] size default to 1 for non-array variables
 * @return true if the variable is a number inside the type range
 */
template <typename T, int64 min, int64 max>
bool get_signed_integer(lua_State *L, const char8 *name, void *ptr,
                        uint32 size) {
  lua_getglobal(L, name);
  int is_num = 0;
  int64 num = lua_tointegerx(L, -1, &is_num);
  if (!is_num) {
    ERROR_GET_NUMBER(name);
    return false;
  }
  lua_pop(L, -1);
  if ((num >= min) && (num <= max)) {
    *(T *)ptr = (T)num;
    return true;
  } else {
    ERROR_LUA_VAR_RANGE(name);
  }
  return false;
}

/**
 * @brief Extract from the Lua stack the the signed integer array (Lua table)
 * identified by its name
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the variable
 * @param[in] ptr pointer to the GAM output signal
 * @param[in] size array size
 * @return true if the variable is a table and each element is number inside the
 * type range
 */
template <typename T, int64 min, int64 max>
bool get_signed_integer_array(lua_State *L, const char8 *name, void *ptr,
                              uint32 size) {
  lua_getglobal(L, name);
  if (!lua_istable(L, -1)) {
    ERROR_GET_TABLE(name);
    return false;
  }
  T *array = (T *)ptr;
  for (uint32 i = 0; i < size; i++) {
    lua_rawgeti(L, -1, i + 1);
    int is_num = 0;
    int64 num = lua_tointegerx(L, -1, &is_num);
    if (is_num) {
      if ((num >= min) && (num <= max)) {
        array[i] = (T)num;
      } else {
        ERROR_LUA_TAB_RANGE(name, i);
        return false;
      }
    } else {
      ERROR_GET_NUMBER_ELEMENT(name, i);
      return false;
    }
    lua_pop(L, -2);
  }
  lua_pop(L, -1);
  return true;
}

/**
 * @brief Extract from the Lua stack the unsigned integer variable identified by
 * its name
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the variable
 * @param[in] ptr pointer to the GAM output signal
 * @param[in] size default to 1 for non-array variables
 * @return true if the variable is a number inside the type range
 */
template <typename T, uint64 max>
bool get_unsigned_integer(lua_State *L, const char8 *name, void *ptr,
                          uint32 size) {
  lua_getglobal(L, name);
  int is_num = 0;
  uint64 num = lua_tointegerx(L, -1, &is_num);
  if (!is_num) {
    ERROR_GET_NUMBER(name);
    return false;
  }
  lua_pop(L, -1);
  if ((num >= 0) && (num <= max)) {
    *(T *)ptr = (T)num;
    return true;
  } else {
    ERROR_LUA_VAR_RANGE(name);
  }
  return false;
}

/**
 * @brief Extract from the Lua stack the the unsigned integer array (Lua table)
 * identified by its name
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the variable
 * @param[in] ptr pointer to the GAM output signal
 * @param[in] size array size
 * @return true if the variable is a table and each element is number inside the
 * type range
 */
template <typename T, uint64 max>
bool get_unsigned_integer_array(lua_State *L, const char8 *name, void *ptr,
                                uint32 size) {
  lua_getglobal(L, name);
  if (!lua_istable(L, -1)) {
    ERROR_GET_TABLE(name);
    return false;
  }
  T *array = (T *)ptr;
  for (uint32 i = 0; i < size; i++) {
    lua_rawgeti(L, -1, i + 1);
    int is_num = 0;
    uint64 num = lua_tointegerx(L, -1, &is_num);
    if (is_num) {
      if ((num >= 0) && (num <= max)) {
        array[i] = (T)num;
      } else {
        ERROR_LUA_TAB_RANGE(name, i);
        return false;
      }
    } else {
      ERROR_GET_NUMBER_ELEMENT(name, i);
      return false;
    }
    lua_pop(L, -1);
  }
  lua_pop(L, -1);
  return true;
}

/**
 * @brief Extract from the Lua stack the boolean variable identified by its name
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the variable
 * @param[in] ptr pointer to the GAM output signal
 * @param[in] size default to 1 for non-array variables
 * @return true if the variable is 0 or 1
 */
bool get_boolean(lua_State *L, const char8 *name, void *ptr, uint32 size) {
  lua_getglobal(L, name);
  if (!lua_isnumber(L, -1)) {
    ERROR_GET_NUMBER(name);
    return false;
  }
  bool num = (bool)lua_toboolean(L, -1);
  lua_pop(L, -1);
  if ((num == 0) || (num == 1)) {
    *(bool *)ptr = num;
    return true;
  } else {
    ERROR_LUA_VAR_RANGE(name);
  }
  return false;
}

/**
 * @brief Extract from the Lua stack the boolean array (Lua table) identified by
 * its name
 * @param[in] L Lua state containing the stack
 * @param[in] name name of the variable
 * @param[in] ptr pointer to the GAM output signal
 * @param[in] size array size
 * @return true if the variable is a table and each element is 0 or 1
 */
bool get_boolean_array(lua_State *L, const char8 *name, void *ptr,
                       uint32 size) {
  lua_getglobal(L, name);
  if (!lua_istable(L, -1)) {
    ERROR_GET_TABLE(name);
    return false;
  }
  bool *array = (bool *)ptr;
  for (uint32 i = 0; i < size; i++) {
    lua_rawgeti(L, -1, i + 1);
    bool num = (bool)lua_toboolean(L, -1);
    if (lua_isnumber(L, -1)) {
      if ((num == 0) || (num == 1)) {
        array[i] = num;
      } else {
        ERROR_LUA_TAB_RANGE(name, i);
        return false;
      }
    } else {
      ERROR_GET_NUMBER_ELEMENT(name, i);
      return false;
    }
    lua_pop(L, -2);
  }
  lua_pop(L, -1);
  return true;
}

const MARTe::uint32 NUM_PARAMS = 1u;
const EC::Parameter parameters[NUM_PARAMS] = {
    EC::Parameter("Code", MARTe::CharString),
};

LuaGAM::LuaGAM() : GAM(), MessageI() {
  code = NULL_PTR(char8 *);
  file_path = NULL_PTR(char8*);
  L = NULL_PTR(lua_State *);
  inputs_functions = NULL_PTR(push_signal *);
  inputs_pointers = NULL_PTR(void **);
  inputs_sizes = NULL_PTR(uint32 *);
  inputs_sig_names = NULL_PTR(char8 **);
  outputs_functions = NULL_PTR(get_signal *);
  outputs_pointers = NULL_PTR(void **);
  outputs_sizes = NULL_PTR(uint32 *);
  outputs_sig_names = NULL_PTR(char8 **);
}

LuaGAM::~LuaGAM() {
  if (code != NULL_PTR(char8 *)) {
    delete[] code;
  }
  if (L != NULL_PTR(lua_State *)) {
    lua_close(L);
  }
  if (inputs_functions != NULL_PTR(push_signal *)) {
    delete[] inputs_functions;
  }
  if (inputs_pointers != NULL_PTR(void **)) {
    delete[] inputs_pointers;
  }
  if (inputs_sizes != NULL_PTR(uint32 *)) {
    delete[] inputs_sizes;
  }
  if (inputs_sig_names != NULL_PTR(char8 **)) {
    for (uint32 i = 0; i < numberOfInputSignals; i++) {
      delete[] inputs_sig_names[i];
    }
    delete[] inputs_sig_names;
  }
  if (outputs_functions != NULL_PTR(get_signal *)) {
    delete[] outputs_functions;
  }
  if (outputs_pointers != NULL_PTR(void **)) {
    delete[] outputs_pointers;
  }
  if (outputs_sizes != NULL_PTR(uint32 *)) {
    delete[] outputs_sizes;
  }
  if (outputs_sig_names != NULL_PTR(char8 **)) {
    for (uint32 i = 0; i < numberOfOutputSignals; i++) {
      delete[] outputs_sig_names[i];
    }
    delete[] outputs_sig_names;
  }
  if (file_path != NULL_PTR(char8*)){
    delete[] file_path;
  }
}

bool LuaGAM::IsCodeExternal() {
  return file_path != NULL_PTR(char8*);
}

Option<const char8*> LuaGAM::CodePath(){
  if (IsCodeExternal()){
    return file_path;
  }
  return Option<const char8*>();
}

bool LuaGAM::Initialise(StructuredDataI &data) {
  bool ok = GAM::Initialise(data);
  StreamString code_str;
  AnyType params_p[NUM_PARAMS] = {code_str};
  ok &= EC::validate_parameters(data, parameters, params_p, NUM_PARAMS);
  uint32 code_str_size = 0;
  if (ok) {
    if (StringHelper::CompareN(code_str.Buffer(), "file://", 7) == 0) {

      uint32 file_path_size = code_str.Size();
      file_path = new char8[file_path_size];
      memset(file_path, 0, file_path_size);
      StringHelper::Copy(file_path, code_str.Buffer() + 7);
      FILE *file = fopen(file_path, "r");
      if (file != NULL) {
        fseek(file, 0, SEEK_END);
        code_str_size = ftell(file);
        rewind(file);
        code = new char8[code_str_size + 1];
        memset(code, 0, code_str_size + 1);
        fread(code, 1, code_str_size, file);
        fclose(file);
      } else {
        REPORT_ERROR(ErrorManagement::InitialisationError,
                     "Error while reading file `%s`\n", file_path);
        ok = false;
      }
    } else {
      code_str_size = code_str.Size() + 1;
      code = new char8[code_str_size];
      memset(code, 0, code_str_size);
      ok &= StringHelper::Copy(code, code_str.Buffer());
    }
    if (ok) {
      L = luaL_newstate();
      ok &= init(L);
      ok &= luaL_dostring(L, code) == LUA_OK;
      if (!ok) {
        REPORT_ERROR(ErrorManagement::InitialisationError,
                     "Lua initialization error: `%s`", lua_tostring(L, -1));
        REPORT_ERROR(ErrorManagement::InitialisationError, "Lua code:\n%s",
                     code);
      } else {
        uint32 stackSize = lua_gettop(L);
        lua_pop(L, stackSize);
      }
    }
  }
  if (ok) {
    lua_getglobal(L, GAM_FN);
    ok &= lua_isfunction(L, -1);
    lua_pop(L, -1);
    if (!ok) {
      REPORT_ERROR(ErrorManagement::InitialisationError,
                   "Function `" GAM_FN "` not present in the Lua code");
    }
  }
  ok &= init_internals_states(data);
  if (!ok) {
    REPORT_ERROR(ErrorManagement::InitialisationError,
                 "Internal states initalisation failed");
  }
  ok &= init_auxiliaries(data);
  if (!ok) {
    REPORT_ERROR(ErrorManagement::InitialisationError,
                 "Auxiliries initalisation failed");
  }
  if (ok && code) {
    ast = LUA::parse(code, ok);
  }

  REPORT_ERROR(ok ? ErrorManagement::Information
                   : ErrorManagement::InitialisationError,
               ok ? "%s Initialised" : "%s Failed to initialise", GetName());
  return ok;
}

void safe_strncpy(char *trg, const char *src, const uint32 n) {
  if (memccpy(trg, src, 0, n) == NULL) {
    trg[n - 1] = 0;
  }
}

bool LuaGAM::Setup() {
  bool ok = code != NULL_PTR(char8 *);
  for (uint32 i = 0; ok && i < GetNumberOfInputSignals(); i++) {
    StreamString in_name;
    ok &= GetSignalName(InputSignals, i, in_name);
    for (uint32 j = 0; ok && j < GetNumberOfOutputSignals(); j++) {
      StreamString out_name;
      ok &= GetSignalName(OutputSignals, j, out_name);
      if (out_name == in_name) {
        ok = false;
        REPORT_ERROR(ErrorManagement::InitialisationError,
                     "Input signal `%s` has same name of output signal: this is not allowed",
                     in_name.Buffer());
      }
    }
  }
  
  LUA::Verifier::LuaGAMValidator validator(ast);
  ok &= validator.check_gam();
  if (ok && !IsCodeExternal()){
    ok &= validator.check_only_gam();
  }
  if (ok && signalsDatabase.MoveRelative("InputSignals")) {
    inputs_functions = new push_signal[numberOfInputSignals];
    inputs_pointers = new void *[numberOfInputSignals];
    inputs_sig_names = new char8 *[numberOfInputSignals];
    inputs_sizes = new uint32[numberOfInputSignals];
    for (uint32 i = 0; i < numberOfInputSignals && ok; i++) {
      TypeDescriptor td = GetSignalType(InputSignals, i);
      uint32 n = 1, m = 1;
      GetSignalNumberOfDimensions(InputSignals, i, n);
      GetSignalNumberOfElements(InputSignals, i, m);
      inputs_sizes[i] = n * m;
      inputs_pointers[i] = GetInputSignalMemory(i);
      const char8 *name = signalsDatabase.GetChildName(i);
      const uint32 len = StringHelper::Length(name) + 1;
      inputs_sig_names[i] = new char8[len];
      safe_strncpy(inputs_sig_names[i], name, len);
      ok &= add_input(i, td);
      ok &= validator.validate_input_signal(inputs_sig_names[i]);
    }
    ok &= signalsDatabase.MoveToAncestor(1u);
  }
  if (ok && signalsDatabase.MoveRelative("OutputSignals")) {
    outputs_functions = new get_signal[numberOfOutputSignals];
    outputs_pointers = new void *[numberOfOutputSignals];
    outputs_sig_names = new char8 *[numberOfOutputSignals];
    outputs_sizes = new uint32[numberOfOutputSignals];
    uint32 max_lines = StringHelper::Length(code) + 1;
    for (uint32 i = 0; i < numberOfOutputSignals && ok; i++) {
      TypeDescriptor td = GetSignalType(OutputSignals, i);
      uint32 n = 1, m = 1;
      GetSignalNumberOfDimensions(OutputSignals, i, n);
      GetSignalNumberOfElements(OutputSignals, i, m);
      outputs_sizes[i] = n * m;
      outputs_pointers[i] = GetOutputSignalMemory(i);
      const char *name = signalsDatabase.GetChildName(i);
      uint32 len = StringHelper::Length(name) + 1;
      outputs_sig_names[i] = new char8[len];
      safe_strncpy(outputs_sig_names[i], name, len);
      ok &= add_output(i, td);
      ok &= validator.validate_output_signal(outputs_sig_names[i], max_lines);
    }
    ok &= signalsDatabase.MoveToAncestor(1u);
  }
  // if (ok) {
  //   const char **variable_names = new const char
  //       *[numberOfInternals + numberOfInputSignals + numberOfOutputSignals];
  //   uint32 j = 0;
  //   for (uint32 i = 0; i < numberOfInputSignals; i++) {
  //     variable_names[j++] = inputs_sig_names[i];
  //   }
  //   for (uint32 i = 0; i < numberOfOutputSignals; i++) {
  //     variable_names[j++] = outputs_sig_names[i];
  //   }
  //   for (uint32 i = 0; i < numberOfInternals; i++) {
  //     variable_names[j++] = internal_states[i];
  //   }
  //
  //   ok &= validator.check_variables_initialisation(
  //       variable_names,
  //       numberOfInputSignals + numberOfOutputSignals + numberOfInternals);
  //   delete[] variable_names;
  // }

  return ok;
}

bool LuaGAM::Execute() {
  bool ok = true;
  try {
    for (uint32 i = 0; i < numberOfInputSignals; i++) {
      inputs_functions[i](L, inputs_sig_names[i], inputs_pointers[i],
                          inputs_sizes[i]);
    }
    lua_getglobal(L, GAM_FN);
    int err = lua_pcall(L, 0, 0, 0);
    if (err) {
      ok = false;
      if (lua_gettop(L)) {
        REPORT_ERROR(ErrorManagement::Warning, "Error executing GAM code: %s",
                     lua_tostring(L, -1));
        lua_pop(L, -1);
      } else {
        REPORT_ERROR(ErrorManagement::Warning, "Error executing GAM code");
      }
    } else {
      for (uint32 i = 0; i < numberOfOutputSignals && ok; i++) {
        ok &= outputs_functions[i](L, outputs_sig_names[i], outputs_pointers[i],
                                   outputs_sizes[i]);
      }
    }
  } catch (char8) {
    REPORT_ERROR(ErrorManagement::Warning, "Error executing GAM code: `%s`",
                 lua_tostring(L, -1));
    ok = false;
  }
  return ok;
}

bool LuaGAM::add_input(uint32 index, const TypeDescriptor td) {
  bool ok = true;
  if (inputs_sizes[index] <= 1) {
    if (td == Float32Bit) {
      inputs_functions[index] = push_float<float32>;
    } else if (td == Float64Bit) {
      inputs_functions[index] = push_float<float64>;
    } else if (td == UnsignedInteger8Bit) {
      inputs_functions[index] = push_integer<uint8>;
    } else if (td == UnsignedInteger16Bit) {
      inputs_functions[index] = push_integer<uint16>;
    } else if (td == UnsignedInteger32Bit) {
      inputs_functions[index] = push_integer<uint32>;
    } else if (td == UnsignedInteger64Bit) {
      REPORT_ERROR_OVERFLOW(inputs_sig_names[index], "UnsignedInteger64Bit");
      inputs_functions[index] = push_integer<uint64>;
    } else if (td == SignedInteger8Bit) {
      inputs_functions[index] = push_integer<int8>;
    } else if (td == SignedInteger16Bit) {
      inputs_functions[index] = push_integer<int16>;
    } else if (td == SignedInteger32Bit) {
      inputs_functions[index] = push_integer<int32>;
    } else if (td == SignedInteger64Bit) {
      REPORT_ERROR_OVERFLOW(inputs_sig_names[index], "SignedInteger64Bit");
      inputs_functions[index] = push_integer<int64>;
    } else if (td == BooleanType) {
      inputs_functions[index] = push_boolean;
    } else {
      REPORT_ERROR(ErrorManagement::InitialisationError,
                   "Input signal type not supported");
      ok = false;
    }
  } else {
    if (td == Float32Bit) {
      inputs_functions[index] = push_float_array<float32>;
    } else if (td == Float64Bit) {
      inputs_functions[index] = push_float_array<float64>;
    } else if (td == UnsignedInteger8Bit) {
      inputs_functions[index] = push_integer_array<uint8>;
    } else if (td == UnsignedInteger16Bit) {
      inputs_functions[index] = push_integer_array<uint16>;
    } else if (td == UnsignedInteger32Bit) {
      inputs_functions[index] = push_integer_array<uint32>;
    } else if (td == UnsignedInteger64Bit) {
      REPORT_ERROR_OVERFLOW(inputs_sig_names[index], "UnsignedInteger64Bit");
      inputs_functions[index] = push_integer_array<uint64>;
    } else if (td == SignedInteger8Bit) {
      inputs_functions[index] = push_integer_array<int8>;
    } else if (td == SignedInteger16Bit) {
      inputs_functions[index] = push_integer_array<int16>;
    } else if (td == SignedInteger32Bit) {
      inputs_functions[index] = push_integer_array<int32>;
    } else if (td == SignedInteger64Bit) {
      REPORT_ERROR_OVERFLOW(inputs_sig_names[index], "SignedInteger64Bit");
      inputs_functions[index] = push_integer_array<int64>;
    } else if (td == BooleanType) {
      inputs_functions[index] = push_boolean_array;
    } else {
      REPORT_ERROR(ErrorManagement::InitialisationError,
                   "Input signal type not supported");
      ok = false;
    }
  }
  return ok;
}

bool LuaGAM::add_output(uint32 index, const TypeDescriptor td) {
  bool ok = true;
  if (outputs_sizes[index] <= 1) {
    if (td == Float32Bit) {
      outputs_functions[index] = get_float<float32>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &=
            push_float<float32>(L, outputs_sig_names[index],
                                outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == Float64Bit) {
      outputs_functions[index] = get_float<float64>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &=
            push_float<float64>(L, outputs_sig_names[index],
                                outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == UnsignedInteger8Bit) {
      outputs_functions[index] = get_unsigned_integer<uint8, UINT8_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &=
            push_integer<uint8>(L, outputs_sig_names[index],
                                outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == UnsignedInteger16Bit) {
      outputs_functions[index] = get_unsigned_integer<uint16, UINT16_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &=
            push_integer<uint16>(L, outputs_sig_names[index],
                                 outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == UnsignedInteger32Bit) {
      outputs_functions[index] = get_unsigned_integer<uint32, UINT32_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &=
            push_integer<uint32>(L, outputs_sig_names[index],
                                 outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == UnsignedInteger64Bit) {
      REPORT_ERROR_OVERFLOW(outputs_sig_names[index], "UnsignedInteger64Bit");
      outputs_functions[index] = get_unsigned_integer<uint64, LUA_MAX_INTEGER>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        // ok &= push_integer<uint64>(L, outputs_sig_names[index],
        // outputs_pointers[index], outputs_sizes[index]);
        ok &=
            push_integer<uint32>(L, outputs_sig_names[index],
                                 outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == SignedInteger8Bit) {
      outputs_functions[index] = get_signed_integer<int8, INT8_MIN, INT8_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_integer<int8>(L, outputs_sig_names[index],
                                 outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == SignedInteger16Bit) {
      outputs_functions[index] =
          get_signed_integer<int16, INT16_MIN, INT16_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &=
            push_integer<int16>(L, outputs_sig_names[index],
                                outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == SignedInteger32Bit) {
      outputs_functions[index] =
          get_signed_integer<int32, INT32_MIN, INT32_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &=
            push_integer<int32>(L, outputs_sig_names[index],
                                outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == SignedInteger64Bit) {
      REPORT_ERROR_OVERFLOW(outputs_sig_names[index], "SignedInteger64Bit");
      outputs_functions[index] =
          get_signed_integer<int64, -LUA_MAX_INTEGER, LUA_MAX_INTEGER>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &=
            push_integer<int64>(L, outputs_sig_names[index],
                                outputs_pointers[index], outputs_sizes[index]);
      }
    } else if (td == BooleanType) {
      outputs_functions[index] = get_boolean;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_boolean(L, outputs_sig_names[index], outputs_pointers[index],
                           outputs_sizes[index]);
      }
    } else {
      REPORT_ERROR(ErrorManagement::InitialisationError,
                   "Output signal type not supported");
      ok = false;
    }
  } else {
    if (td == Float32Bit) {
      outputs_functions[index] = get_float_array<float32>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_float_array<float32>(L, outputs_sig_names[index],
                                        outputs_pointers[index],
                                        outputs_sizes[index]);
      }
    } else if (td == Float64Bit) {
      outputs_functions[index] = get_float_array<float64>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_float_array<float64>(L, outputs_sig_names[index],
                                        outputs_pointers[index],
                                        outputs_sizes[index]);
      }
    } else if (td == UnsignedInteger8Bit) {
      outputs_functions[index] = get_unsigned_integer_array<uint8, UINT8_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_integer_array<uint8>(L, outputs_sig_names[index],
                                        outputs_pointers[index],
                                        outputs_sizes[index]);
      }
    } else if (td == UnsignedInteger16Bit) {
      outputs_functions[index] = get_unsigned_integer_array<uint16, UINT16_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_integer_array<uint16>(L, outputs_sig_names[index],
                                         outputs_pointers[index],
                                         outputs_sizes[index]);
      }
    } else if (td == UnsignedInteger32Bit) {
      outputs_functions[index] = get_unsigned_integer_array<uint32, UINT32_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_integer_array<uint32>(L, outputs_sig_names[index],
                                         outputs_pointers[index],
                                         outputs_sizes[index]);
      }
    } else if (td == UnsignedInteger64Bit) {
      REPORT_ERROR_OVERFLOW(outputs_sig_names[index], "UnsignedInteger64Bit");
      outputs_functions[index] =
          get_unsigned_integer_array<uint64, LUA_MAX_INTEGER>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        // ok &= push_integer_array<uint64>(L, outputs_sig_names[index],
        // outputs_pointers[index], outputs_sizes[index]);
        ok &= push_integer_array<uint32>(L, outputs_sig_names[index],
                                         outputs_pointers[index],
                                         outputs_sizes[index]);
      }
    } else if (td == SignedInteger8Bit) {
      outputs_functions[index] =
          get_signed_integer_array<int8, INT8_MIN, INT8_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_integer_array<int8>(L, outputs_sig_names[index],
                                       outputs_pointers[index],
                                       outputs_sizes[index]);
      }
    } else if (td == SignedInteger16Bit) {
      outputs_functions[index] =
          get_signed_integer_array<int16, INT16_MIN, INT16_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_integer_array<int16>(L, outputs_sig_names[index],
                                        outputs_pointers[index],
                                        outputs_sizes[index]);
      }
    } else if (td == SignedInteger32Bit) {
      outputs_functions[index] =
          get_signed_integer_array<int32, INT32_MIN, INT32_MAX>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_integer_array<int32>(L, outputs_sig_names[index],
                                        outputs_pointers[index],
                                        outputs_sizes[index]);
      }
    } else if (td == SignedInteger64Bit) {
      REPORT_ERROR_OVERFLOW(outputs_sig_names[index], "SignedInteger64Bit");
      outputs_functions[index] =
          get_signed_integer_array<int64, -LUA_MAX_INTEGER, LUA_MAX_INTEGER>;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        ok &= push_integer_array<int64>(L, outputs_sig_names[index],
                                        outputs_pointers[index],
                                        outputs_sizes[index]);
      }
    } else if (td == BooleanType) {
      outputs_functions[index] = get_boolean_array;
      if (outputs_pointers[index] != NULL_PTR(void *)) {
        push_boolean_array(L, outputs_sig_names[index], outputs_pointers[index],
                           outputs_sizes[index]);
      }
    } else {
      REPORT_ERROR(ErrorManagement::InitialisationError,
                   "Output signal type not supported");
      ok = false;
    }
  }
  return ok;
}

bool LuaGAM::init_internals_states(StructuredDataI &data) {
  bool ok = true;
  if (data.MoveRelative("InternalStates")) {
    uint32 numberOfInternals = data.GetNumberOfChildren();
    for (uint32 i = 0u; (i < numberOfInternals) && ok; i++) {
      StreamString value;
      const char8 *name = data.GetChildName(i);
      uint32 len = StringHelper::Length(name) + 1;
      ok &= data.Read(name, value);
      if (!ok) {
        REPORT_ERROR(ErrorManagement::InitialisationError,
                     "Error reading internal state `%s`.\n", name);
        break;
      }
      char8 *internal_code = new char8[MAX_LENGTH];
      memset(internal_code, 0, MAX_LENGTH);
      ok &= StringHelper::Copy(internal_code, name);
      ok &= StringHelper::Concatenate(internal_code, "=");
      ok &= StringHelper::Concatenate(internal_code, value.Buffer());
      LUA::parse(internal_code, ok);
      ok &= luaL_dostring(L, internal_code) == LUA_OK;
      if (!ok) {
        REPORT_ERROR(ErrorManagement::InitialisationError,
                     "Error in internal state `%s` code `%s`.\n", name,
                     internal_code);
      }
      delete[] internal_code;
    }
    ok &= data.MoveToAncestor(1u);
  } else {
    REPORT_ERROR(ErrorManagement::Information, "No internal states set.");
  }
  return ok;
}

bool LuaGAM::init_auxiliaries(StructuredDataI &data) {
  bool ok = true;
  if (data.MoveRelative("AuxiliaryFunctions")) {
    uint32 numberOfChildren = data.GetNumberOfChildren();
    for (uint32 i = 0u; (i < numberOfChildren) && ok; i++) {
      StreamString fun;
      const char8 *name = data.GetChildName(i);
      ok &= data.Read(name, fun);
      char8 *auxiliary_code = new char8[fun.Size() + 1];
      safe_strncpy(auxiliary_code, fun.Buffer(), fun.Size() + 1);
      LUA::parse(auxiliary_code, ok);
      ok &= luaL_dostring(L, auxiliary_code) == LUA_OK;
      if (!ok) {
        REPORT_ERROR(
            ErrorManagement::InitialisationError,
            "Error encountered in auxiliary function `%s` code `%s`.\n", name,
            auxiliary_code);
      }
      delete[] auxiliary_code;
    }
    ok &= data.MoveToAncestor(1u);
  } else {
    REPORT_ERROR(ErrorManagement::Information, "No auxiliaries set.");
  }
  return ok;
}

CLASS_REGISTER(LuaGAM, "1.0")
} /* namespace MARTe */
