#include "ConfigurationDatabase.h"
#include "ErrorType.h"
#include "JSONLogger.h"
#include "LoggerService.h"
#include "gtest/gtest.h"

TEST(JSONLogger, Init) {
  MARTe::JSONLogger logger;
  MARTe::ConfigurationDatabase cdb;
  cdb.Write("LogPath", "/tmp/test");
  cdb.Write("UniqueFile", true);
  ASSERT_TRUE(logger.Initialise(cdb));
}

TEST(JSONLogger, ConsumeLog) {
  using namespace MARTe;
  LoggerService test;
  ReferenceT<JSONLogger> consumer(
      GlobalObjectsDatabase::Instance()->GetStandardHeap());
  ConfigurationDatabase cdb2;
  cdb2.Write("LogPath", "/tmp/test");
  ASSERT_TRUE(consumer->Initialise(cdb2));
  ConfigurationDatabase cdb;
  test.Insert(consumer);
  cdb.Write("CPUs", 0x2);
  ASSERT_TRUE(test.Initialise(cdb));
  REPORT_ERROR_STATIC(ErrorManagement::Debug, "TestConsumeLogMessage");
  REPORT_ERROR_STATIC(ErrorManagement::Information, "TestConsumeLogMessage");
  REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                      "TestConsumeLogMessage");
  Sleep::Sec(0.1);
  ASSERT_TRUE(true);
}
TEST(JSONLogger, ConsumeStdOutLog) {
  using namespace MARTe;
  LoggerService test;
  ReferenceT<JSONLogger> consumer(
      GlobalObjectsDatabase::Instance()->GetStandardHeap());
  ConfigurationDatabase cdb2;
  cdb2.Write("LogPath", "stdout");
  ASSERT_TRUE(consumer->Initialise(cdb2));
  ConfigurationDatabase cdb;
  test.Insert(consumer);
  cdb.Write("CPUs", 0x2);
  ASSERT_TRUE(test.Initialise(cdb));
  REPORT_ERROR_STATIC(ErrorManagement::Debug, "TestConsumeLogMessage");
  REPORT_ERROR_STATIC(ErrorManagement::Information, "TestConsumeLogMessage");
  REPORT_ERROR_STATIC(ErrorManagement::InitialisationError,
                      "TestConsumeLogMessage");
  Sleep::Sec(0.1);
  ASSERT_TRUE(true);
}
TEST(JSONLogger, SanitizeLog) {
  using namespace MARTe;
  LoggerService test;
  ReferenceT<JSONLogger> consumer(
      GlobalObjectsDatabase::Instance()->GetStandardHeap());
  ConfigurationDatabase cdb2;
  cdb2.Write("LogPath", "stdout");
  ASSERT_TRUE(consumer->Initialise(cdb2));
  ConfigurationDatabase cdb;
  test.Insert(consumer);
  cdb.Write("CPUs", 0x2);
  ASSERT_TRUE(test.Initialise(cdb));
  REPORT_ERROR_STATIC(ErrorManagement::Debug, "Multiple line\n string");
  REPORT_ERROR_STATIC(ErrorManagement::Information, "\t\t\t tabs string");
  REPORT_ERROR_STATIC(ErrorManagement::InitialisationError, "Quted \"string\"");
  Sleep::Sec(0.1);
  ASSERT_TRUE(true);
}
