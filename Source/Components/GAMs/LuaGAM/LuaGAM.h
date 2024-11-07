/**
 * @file LuaGAM.h
 * @brief Header file for class Lua GAM
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

 * @details This header file contains the declaration of the class
 * LuaGAM with all of its public, protected and private
 * members. It may also include definitions for inline methods which need to be
 * visible to the compiler.
 */

#ifndef LUA_GAM_H
#define LUA_GAM_H

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/
#include "AST.h"
#include "ec-types/include/optional.h"
#include "lua.hpp"

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

#include "GAM.h"
#include "MessageI.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

#define MAX_LENGTH 256
#define LUA_MAX_INTEGER 9007199254740991 // 2**53 - 1

namespace MARTe {

typedef bool (*push_signal)(lua_State *, const char8 *, void *, uint32);
typedef bool (*get_signal)(lua_State *, const char8 *, void *, uint32);

/**
 * {
 *   Class: "LuaGAM"
 *   Code: string
 *   InputSignals: {
 *      ...
 *   }
 *   OutputSignals: {
 *      ...
 *   }
 * }
 **/

class LuaGAM : public GAM, public MessageI {
public:
  CLASS_REGISTER_DECLARATION()

  /**
   * @brief Constructor. NOOP.
   */
  LuaGAM();

  /**
   * @brief Destructor. NOOP.
   */
  virtual ~LuaGAM();

  /**
   * @brief Initialises the output signal memory with default values provided
   * through configuration.
   * @return true if the pre-conditions are met.
   * @pre
   *   SetConfiguredDatabase() && GetNumberOfInputSignals() ==
   * GetNumberOfOutputSignals() for each signal i: The default value provided
   * corresponds to the expected type and dimensionality.
   */
  virtual bool Setup();

  /**
   * @see DataSourceI::Initialise
   * @details Initialise the parameters of the SCPIDataSource.
   **/
  virtual bool Initialise(StructuredDataI &data);

  /**
   * @brief Execute method. NOOP.
   * @return true.
   */
  virtual bool Execute();

  /**
   * @brief Check if code is in an external file or is defined in the
   *configuration.
   * @return true if code is an external file, false otherwise
   **/
  bool IsCodeExternal();

  /**
   * @brief Get code file path, if any.
   * @return file path if any.
   **/
  ect::Optional<const char8 *> CodePath();

private:
  char8 *code;      //!< Lua code
  char8 *file_path; //!< file path of code
  LUA::ast_t ast;        //!< Code AST
  lua_State *L;     //!< lua state

  push_signal *inputs_functions; //!< Array of functions pushing the GAM input
                                 //!< signals in the Lua stack
  void **inputs_pointers;        //!< Array of pointers to the GAM input signals
  uint32 *inputs_sizes;          //!< Array of GAM input signal sizes
                                 //!< (dimensions x elements)
  char8 **inputs_sig_names;      //!< Array of GAM input signal names

  get_signal *outputs_functions; //!< Array of functions retrieving the GAm
                                 //!< output signals from the Lua stack
  void **outputs_pointers;       //!< Array of pointers to the GAM output
                                 //!< signals
  uint32 *outputs_sizes;         //!< Array of GAM output signal sizes
                                 //!< (dimensions x elements)
  char8 **outputs_sig_names;     //!< Array of GAM output signal names

  /**
   * @brief Add input signal push function
   * @param[in] index input signal index
   * @param[in] td type of the GAM input signal
   * @return true if signal correctly added
   */
  bool add_input(uint32 index, const TypeDescriptor td);

  /**
   * @brief Add output signal get function
   * @param[out] index output signal index
   * @param[in] td type of the GAM output signal
   * @return true if signal correctly added
   */
  bool add_output(uint32 index, const TypeDescriptor td);

  /**
   * @brief Initialise Lua global variables
   * @param[in] data GAM StructuredDataI
   * @return true if correct initialization
   */
  bool init_internals_states(StructuredDataI &data);

  /**
   * @brief Initialise Lua auxiliary functions to be used in the main code
   * @param[in] data GAM StructuredDataI
   * @return true if correct initialization
   */
  bool init_auxiliaries(StructuredDataI &data);
};

} // namespace MARTe

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* LUA_GAM_H */
