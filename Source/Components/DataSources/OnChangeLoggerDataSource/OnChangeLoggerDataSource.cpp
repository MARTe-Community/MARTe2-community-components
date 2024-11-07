/**
 * @file LoggerDataSource.cpp
 * @brief Source file for class LoggerDataSource
 * @date 9/11/2016
 * @author Andre Neto
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
 * the class LoggerDataSource (public, protected, and private). Be aware that
 some
 * methods, such as those inline could be defined on the header file, instead.
 */

#define DLL_API

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "OnChangeLoggerDataSource.h"
#include "CompilerTypes.h"
#include "OnChangeBroker.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe {

OnChangeLoggerDataSource::OnChangeLoggerDataSource() : DataSourceI() {
  ignoreMask = NULL_PTR(MARTe::uint8 *);
}

OnChangeLoggerDataSource::~OnChangeLoggerDataSource() {
  if (ignoreMask != NULL_PTR(uint8 *)) {
    delete[] ignoreMask;
  }
}

bool OnChangeLoggerDataSource::Initialise(StructuredDataI &data) {
  bool ret = DataSourceI::Initialise(data);
  return ret;
}

bool OnChangeLoggerDataSource::SetConfiguredDatabase(StructuredDataI &data) {
  bool ret = DataSourceI::SetConfiguredDatabase(data);

  // create mask
  ignoreMask = new MARTe::uint8[numberOfSignals];
  // initalize all signal masks to false (0)
  for (uint32 i = 0; i < numberOfSignals; i++) {
    ignoreMask[i] = 0u;
  }
  // check if some ignore flags are configured in the user configuration
  if (ret) {
    // for each child
    for (uint32 i = 0; i < numberOfSignals && ret; i++) {
      ret = signalsDatabase.MoveToChild(i);
      if (ret) {
        // initialize local flag to false
        uint8 flag = 0u;
        // try to read the Ignore local setting
        if (signalsDatabase.Read("Ignore", flag)) {
          ignoreMask[i] = flag;
        }
      }
      ret = signalsDatabase.MoveToAncestor(1u);
    }
  }
  return ret;
}

bool OnChangeLoggerDataSource::Synchronise() { return false; }

bool OnChangeLoggerDataSource::AllocateMemory() { return true; }

uint32 OnChangeLoggerDataSource::GetNumberOfMemoryBuffers() { return 0u; }

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification:
 * Method not required by this DataSourceI implementation. (TODO move interface
 * to a MemoryMappedDataSource).*/
bool OnChangeLoggerDataSource::GetSignalMemoryBuffer(const uint32 signalIdx,
                                                     const uint32 bufferIdx,
                                                     void *&signalAddress) {
  return true;
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification:
 * Method not required by this DataSourceI implementation. (TODO move interface
 * to a MemoryMappedDataSource).*/
const char8 *
OnChangeLoggerDataSource::GetBrokerName(StructuredDataI &data,
                                        const SignalDirection direction) {
  return "OnChangeBroker";
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification:
 * Method not required by this DataSourceI implementation. (TODO move interface
 * to a MemoryMappedDataSource).*/
bool OnChangeLoggerDataSource::GetInputBrokers(ReferenceContainer &inputBrokers,
                                               const char8 *const functionName,
                                               void *const gamMemPtr) {
  return false;
}

bool OnChangeLoggerDataSource::GetOutputBrokers(
    ReferenceContainer &outputBrokers, const char8 *const functionName,
    void *const gamMemPtr) {
  ReferenceT<OnChangeBroker> broker("OnChangeBroker");
  bool ok = broker->Init(OutputSignals, *this, functionName, gamMemPtr);
  if (ok) {
    // set broker mask
    broker->SetMask(ignoreMask, numberOfSignals);
    ok = outputBrokers.Insert(broker);
  }
  return ok;
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification:
 * this DataSourceI implementation is independent of the states being changed.*/
bool OnChangeLoggerDataSource::PrepareNextState(
    const char8 *const currentStateName, const char8 *const nextStateName) {
  return true;
}

CLASS_REGISTER(OnChangeLoggerDataSource, "1.0")
} // namespace MARTe
/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
