/**
 * @file UtilsGAM.cpp
 * @brief Source file for class UtilsGAM
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

 * @details This source file contains the definition of all the methods for
 * the class UtilsGAM (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include "TestMacros.h"
/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "ConfigurationDatabase.h"
#include "File.h"
#include "ObjectRegistryDatabase.h"
#include "RealTimeApplication.h"
#include "StandardParser.h"
#include "Utils.h"
#include <sys/time.h>
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
#define BILLION 1000000000
/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

namespace utils {
MARTe::uint64 utime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * (MARTe::uint64)1000000 + tv.tv_usec;
}
timespec ntime(bool &ok) {
  timespec tclk;
  if (clock_gettime(CLOCK_REALTIME, &tclk) != 1) {
    ok = false;
  }
  ok = true;
  return tclk;
}
MARTe::uint64 dtns(timespec start, timespec end) {
  return BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
}
} // namespace utils

/**
 * Helper function to setup a MARTe execution environment
 */
/**
 * Helper function to setup a MARTe execution environment
 */
bool InitialiseEnviroment(const char8 *const config, bool configure) {
  HeapManager::AddHeap(GlobalObjectsDatabase::Instance()->GetStandardHeap());
  ConfigurationDatabase cdb;
  File configStream;
  T_ASSERT_TRUE(configStream.Open(config, BasicFile::ACCESS_MODE_R));
  StandardParser parser(configStream, cdb);
  T_ASSERT_TRUE(parser.Parse());
  ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
  god->Purge();
  T_ASSERT_TRUE(god->Initialise(cdb));
  if (configure) {
    ReferenceT<RealTimeApplication> application;
    application = god->Find("Test");
    T_ASSERT_TRUE(application.IsValid());
    T_ASSERT_TRUE(application->ConfigureApplication());
  }
  return true;
}

bool StartApplication(const MARTe::char8 *const state) {
  ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
  ReferenceT<RealTimeApplication> application = god->Find("Test");

  T_ASSERT_TRUE(application.IsValid());
  T_ASSERT_TRUE(application->PrepareNextState(state));
  T_ASSERT_TRUE(application->StartNextStateExecution());

  return true;
}

bool StopApplication() {
  ObjectRegistryDatabase *god = ObjectRegistryDatabase::Instance();
  ReferenceT<RealTimeApplication> application = god->Find("Test");
  bool ok = application.IsValid();
  if (!ok) {
    REPORT_ERROR_STATIC(ErrorManagement::InternalSetupError,
                        "RealTimeApplication::IsValid failed");
  } else {
    ok = application->StopCurrentStateExecution();
  }
  god->Purge();
  return ok;
}

UtilsGAM::UtilsGAM() : GAM() { numberOfExecutions = 0u; }

UtilsGAM::~UtilsGAM() {}

bool UtilsGAM::Setup() { return true; }

bool UtilsGAM::Execute() {
  numberOfExecutions++;
  return true;
}

void *UtilsGAM::GetInputMemoryBufferX() { return GetInputSignalsMemory(); }

void *UtilsGAM::GetOutputMemoryBufferX() { return GetOutputSignalsMemory(); }

CLASS_REGISTER(UtilsGAM, "1.0")
