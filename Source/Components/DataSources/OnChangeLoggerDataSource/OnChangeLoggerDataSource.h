/**
 * @file OnChangeLoggerDataSource.h
 * @brief Header file for class OnChangeLoggerDataSource
 * @date 03/05/2023
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
 * OnChangeLoggerDataSource with all of its public, protected and private
 * members. It may also include definitions for inline methods which need to
 * be visible to the compiler.
 */

#ifndef ONCHANGELOGGERDATASOURCE_H_
#define ONCHANGELOGGERDATASOURCE_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "DataSourceI.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
namespace MARTe {
/**
@brief A DataSource which prints in the MARTe logger the current value of any
signal.
@details The LoggerDataSource will call REPORT_ERROR with the current value of
 all the signals listed on its Signals section.

The configuration template can be found below:
```cue
{
  Class: "OnChangeLoggerDataSource"
  Signals: {
    [_]: {
      Ignore?:    *0 | 1 // if ignored will not trigger a print
    }
    ...
  }
}
```
**/
class OnChangeLoggerDataSource : public DataSourceI {
public:
  CLASS_REGISTER_DECLARATION()
  /**
   * @brief Default constructor. NOOP.
   */
  OnChangeLoggerDataSource();

  /**
   * @brief Destructor. NOOP.
   */
  virtual ~OnChangeLoggerDataSource();

  virtual bool Initialise(StructuredDataI &data);

  /**
   * @brief NOOP.
   * @return true.
   */
  virtual bool Synchronise();

  /**
   * @brief NOOP.
   * @return true.
   */
  virtual bool AllocateMemory();

  /**
   * @see MemoryDataSourceI::SetConfiguredDatabase
   * @details prepare ignore mask array
   */
  virtual bool SetConfiguredDatabase(StructuredDataI &data);
  /**
   * @brief NOOP.
   * @return 0u.
   */
  virtual uint32 GetNumberOfMemoryBuffers();

  /**
   * @brief NOOP.
   * @return true.
   */
  virtual bool GetSignalMemoryBuffer(const uint32 signalIdx,
                                     const uint32 bufferIdx,
                                     void *&signalAddress);

  /**
   * @brief Returns "LoggerBroker".
   * @param[in] data is not used as LoggerBroker is always returned.
   * @param[in] direction is not used as LoggerBroker is always returned.
   * @return "LoggerBroker"
   */
  virtual const char8 *GetBrokerName(StructuredDataI &data,
                                     const SignalDirection direction);

  /**
   * @brief Only OutputBrokers are supported.
   * @return false.
   */
  virtual bool GetInputBrokers(ReferenceContainer &inputBrokers,
                               const char8 *const functionName,
                               void *const gamMemPtr);

  /**
   * @brief Creates a new instance of a LoggerBroker and adds it to \a
   * outputBrokers.
   * @param[out] outputBrokers where the BrokerI instances have to be added to.
   * @param[in] functionName name of the function being queried.
   * @param[in] gamMemPtr the GAM memory where the signals will be written to.
   * @return true iff the LoggerBroker has been successfully initialised and
   * added to the \a outputBrokers.
   */
  virtual bool GetOutputBrokers(ReferenceContainer &outputBrokers,
                                const char8 *const functionName,
                                void *const gamMemPtr);

  /**
   * @brief NOOP.
   * @return true.
   */
  virtual bool PrepareNextState(const char8 *const currentStateName,
                                const char8 *const nextStateName);

protected:
  /**
   * @brief array containing ignore flags
   */
  uint8 *ignoreMask;
};
} // namespace MARTe

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* ONCHANGELOGGERDATASOURCE_H_ */
