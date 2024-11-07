#include "MessageInterfaceTest.h"
#include "CLASSMETHODREGISTER.h"
#include "ConfigurationDatabase.h"
#include "HttpChunkedStream.h"
#include "HttpClient.h"
#include "HttpDefinition.h"
#include "HttpDynamicMessageInterface.h"
#include "HttpProtocol.h"
#include "HttpService.h"
#include "ObjectRegistryDatabase.h"
#include "RegisteredMethodsMessageFilter.h"
#include "StandardParser.h"
#include "StreamString.h"
#include "ec-types/include/logging.h"
#include "ec-types/test/macros.h"

/*---------------------------------------------------------------------------*/
class HttpMessageInterfaceTestObject : public MARTe::Object,
                                       public MARTe::MessageI {
public:
  CLASS_REGISTER_DECLARATION()

  HttpMessageInterfaceTestObject() {
    MARTe::ReferenceT<MARTe::RegisteredMethodsMessageFilter> filter =
        MARTe::ReferenceT<MARTe::RegisteredMethodsMessageFilter>(
            MARTe::GlobalObjectsDatabase::Instance()->GetStandardHeap());
    filter->SetDestination(this);
    InstallMessageFilter(filter);
    flag = 0;
  }

  MARTe::ErrorManagement::ErrorType
  ReceiverMethod(MARTe::ReferenceContainer &msg) {
    MARTe::ReferenceT<MARTe::StructuredDataI> params = msg.Get(0u);
    MARTe::int32 i = 1;
    if (params.IsValid()) {
      if (!params->Read("Delta", i)) {
        i = 1;
      }
    }
    flag += i;
    return MARTe::ErrorManagement::NoError;
  }

  MARTe::int32 flag;
};
CLASS_REGISTER(HttpMessageInterfaceTestObject, "1.0")
CLASS_METHOD_REGISTER(HttpMessageInterfaceTestObject, ReceiverMethod)

/*****************************************************************************/

bool MessageInterfaceTester::TestInit() {
  bool ok = true;
  MARTe::HttpDynamicMessageInterface messageInterface;
  MARTe::ConfigurationDatabase cdb;
  EC_ASSERT_TRUE(messageInterface.Initialise(cdb));
  return ok;
}

bool MessageInterfaceTester::TestEmptyMessage() {
  bool ok = true;
  MARTe::HttpDynamicMessageInterface messageInterface;
  MARTe::ConfigurationDatabase cdb;
  EC_ASSERT_TRUE(messageInterface.Initialise(cdb));
  MARTe::HttpChunkedStream stream;
  MARTe::HttpProtocol protocol(stream);

  MARTe::StreamString str;
  EC_ASSERT_FALSE(messageInterface.GetAsText(str, protocol));

  return ok;
}

#define ASSERT_AND_PURGE(x)                                                    \
  {                                                                            \
    if (!(x)) {                                                                \
      ect::log::error("Condition `" #x "` at " __FILE__ ":%d is not true\n",   \
                      __LINE__);                                               \
      ObjectRegistryDatabase::Instance()->Purge();                             \
      return false;                                                            \
    }                                                                          \
  }

bool MessageInterfaceTester::TestTextReplay() {
  using namespace MARTe;
  StreamString cfg = ""
                     "+TestObj = {\n"
                     "    Class = HttpMessageInterfaceTestObject\n"
                     "}\n"
                     "+TestObj2 = {\n"
                     "    Class = HttpMessageInterfaceTestObject\n"
                     "}\n"
                     "+HttpService1 = {\n"
                     "    Class = HttpService\n"
                     "    Port = 9094\n"
                     "    Timeout = 0\n"
                     "    AcceptTimeout = 100"
                     "    MinNumberOfThreads = 1\n"
                     "    MaxNumberOfThreads = 8\n"
                     "    ListenMaxConnections = 255\n"
                     "    IsTextMode = 1\n"
                     "    WebRoot = HttpObjectBrowser1\n"
                     "}\n"
                     "+HttpObjectBrowser1 = {\n"
                     "    Class = HttpObjectBrowser\n"
                     "    Root = \".\""
                     "    +MessageInterface1 = {\n"
                     "        Class = HttpDynamicMessageInterface\n"
                     "    }\n"
                     "}\n";

  cfg.Seek(0LLU);
  StreamString err;
  ConfigurationDatabase cdb;
  StandardParser parser(cfg, cdb, &err);
  ASSERT_AND_PURGE(parser.Parse());
  ASSERT_AND_PURGE(cdb.MoveToRoot());
  ASSERT_AND_PURGE(ObjectRegistryDatabase::Instance()->Initialise(cdb));
  ReferenceT<HttpService> service =
      ObjectRegistryDatabase::Instance()->Find("HttpService1");
  ASSERT_AND_PURGE(service.IsValid());

  ASSERT_AND_PURGE(service->Start());
  HttpClient test;
  test.SetServerAddress("127.0.0.1");
  test.SetServerPort(9094);
  test.SetServerUri("/MessageInterface1");
  StreamString reply;
  ASSERT_AND_PURGE(
      test.HttpExchange(reply, HttpDefinition::HSHCGet, NULL, 1000u));
  const char8 *expectedReply = "0\r\n\r\n";
  ASSERT_AND_PURGE(reply == expectedReply);

  ReferenceT<HttpMessageInterfaceTestObject> target =
      ObjectRegistryDatabase::Instance()->Find("TestObj");
  ReferenceT<HttpMessageInterfaceTestObject> target2 =
      ObjectRegistryDatabase::Instance()->Find("TestObj2");
  ASSERT_AND_PURGE(target.IsValid());
  for (int32 i = 0u; i < 5; i++) {
    test.SetServerUri(
        "/MessageInterface1?destination=TestObj&function=ReceiverMethod");
    StreamString reply;
    ASSERT_AND_PURGE(
        test.HttpExchange(reply, HttpDefinition::HSHCGet, NULL, 1000u));
    ASSERT_AND_PURGE(reply == "0\r\n\r\n");
    ASSERT_AND_PURGE(target->flag == (i + 1));
  }
  for (int32 i = 0; i < 5; i++) {
    test.SetServerUri("/MessageInterface1?destination=TestObj2&function="
                      "ReceiverMethod&Delta=2");
    StreamString reply;
    ASSERT_AND_PURGE(
        test.HttpExchange(reply, HttpDefinition::HSHCGet, NULL, 1000u));
    ASSERT_AND_PURGE(reply == "0\r\n\r\n");
    ASSERT_AND_PURGE(target2->flag == ((i + 1) * 2));
  }
  ASSERT_AND_PURGE(service->Stop());
  ObjectRegistryDatabase::Instance()->Purge();

  return true;
}
bool MessageInterfaceTester::TestStructuredReplay() {
  using namespace MARTe;
  StreamString cfg = ""
                     "+TestObj = {\n"
                     "    Class = HttpMessageInterfaceTestObject\n"
                     "}\n"
                     "+TestObj2 = {\n"
                     "    Class = HttpMessageInterfaceTestObject\n"
                     "}\n"
                     "+HttpService1 = {\n"
                     "    Class = HttpService\n"
                     "    Port = 9094\n"
                     "    Timeout = 0\n"
                     "    AcceptTimeout = 100"
                     "    MinNumberOfThreads = 1\n"
                     "    MaxNumberOfThreads = 8\n"
                     "    ListenMaxConnections = 255\n"
                     "    IsTextMode = 0\n"
                     "    WebRoot = HttpObjectBrowser1\n"
                     "}\n"
                     "+HttpObjectBrowser1 = {\n"
                     "    Class = HttpObjectBrowser\n"
                     "    Root = \".\""
                     "    +MessageInterface1 = {\n"
                     "        Class = HttpDynamicMessageInterface\n"
                     "    }\n"
                     "}\n";

  cfg.Seek(0LLU);
  StreamString err;
  ConfigurationDatabase cdb;
  StandardParser parser(cfg, cdb, &err);
  ASSERT_AND_PURGE(parser.Parse());
  ASSERT_AND_PURGE(cdb.MoveToRoot());
  ASSERT_AND_PURGE(ObjectRegistryDatabase::Instance()->Initialise(cdb));
  ReferenceT<HttpService> service =
      ObjectRegistryDatabase::Instance()->Find("HttpService1");
  ASSERT_AND_PURGE(service.IsValid());

  ASSERT_AND_PURGE(service->Start());
  HttpClient test;
  test.SetServerAddress("127.0.0.1");
  test.SetServerPort(9094);

  ReferenceT<HttpMessageInterfaceTestObject> target =
      ObjectRegistryDatabase::Instance()->Find("TestObj");
  ReferenceT<HttpMessageInterfaceTestObject> target2 =
      ObjectRegistryDatabase::Instance()->Find("TestObj2");
  ASSERT_AND_PURGE(target.IsValid());
  for (int32 i = 0u; i < 5; i++) {
    test.SetServerUri(
        "/MessageInterface1?destination=TestObj&function=ReceiverMethod");
    StreamString reply;
    ASSERT_AND_PURGE(
        test.HttpExchange(reply, HttpDefinition::HSHCGet, NULL, 1000u));
    ASSERT_AND_PURGE(reply == "A\r\n{\n\r\"OK\": 1\r\n1\r\n}\r\n0\r\n\r\n");
    ASSERT_AND_PURGE(target->flag == (i + 1));
  }
  for (int32 i = 0; i < 5; i++) {
    test.SetServerUri("/MessageInterface1?destination=TestObj2&function="
                      "ReceiverMethod&Delta=2");
    StreamString reply;
    ASSERT_AND_PURGE(
        test.HttpExchange(reply, HttpDefinition::HSHCGet, NULL, 1000u));
    ASSERT_AND_PURGE(reply == "A\r\n{\n\r\"OK\": 1\r\n1\r\n}\r\n0\r\n\r\n");
    ASSERT_AND_PURGE(target2->flag == ((i + 1) * 2));
  }
  ASSERT_AND_PURGE(service->Stop());
  ObjectRegistryDatabase::Instance()->Purge();

  return true;
}
