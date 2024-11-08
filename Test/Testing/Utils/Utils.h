/**
 * @file Utils.h
 * @brief Header file for class Utils
 * @date 08/04/2021
 * @author Giuseppe Ferro
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

 * @details This header file contains the declaration of the class Utils
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef UTILS_H_
#define UTILS_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

#include "GAM.h"
namespace utils {
MARTe::uint64 utime(); ///< return time in us
timespec ntime(bool &ok);
MARTe::uint64 dtns(timespec start, timespec end);
} // namespace utils
/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

using namespace MARTe;

/**
 * Helper function to setup a MARTe execution environment
 */
bool InitialiseEnviroment(const char8 *const config, bool configure = true);

bool StartApplication(const char8 *const state = "Running");

bool StopApplication();

class UtilsGAM : public GAM {
  // TODO Add the macro DLL_API to the class declaration (i.e. class DLL_API
  // UtilsGAM)
public:
  CLASS_REGISTER_DECLARATION()

  UtilsGAM();
  virtual ~UtilsGAM();

  virtual bool Setup();

  virtual bool Execute();

  void *GetInputMemoryBufferX();

  void *GetOutputMemoryBufferX();

  uint32 numberOfExecutions;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* SRC_TEST_C___COMPONENTS_GAMS_UTILS_UTILS_H_ */
