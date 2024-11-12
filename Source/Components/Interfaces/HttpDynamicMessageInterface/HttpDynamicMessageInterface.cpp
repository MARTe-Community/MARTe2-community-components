/**
 * @file HttpDynamicMessageInterface.cpp
 * @brief Source file for class HttpDynamicMessageInterface
 * @date 09/12/2024
 * @author Andrea Antonione
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
 * the class HttpDynamicMessageInterface (public, protected, and private). Be
 aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "HttpDynamicMessageInterface.h"
#include "AdvancedErrorManagement.h"
#include "ConfigurationDatabase.h"
#include "ErrorType.h"
#include "JsonPrinter.h"
#include "Message.h"
#include "MessageI.h"
#include "StreamString.h"
#include "StreamStructuredData.h"
#include "StringHelper.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

namespace MARTe {

HttpDynamicMessageInterface::HttpDynamicMessageInterface()
    : HttpMessageInterface() {}

HttpDynamicMessageInterface::~HttpDynamicMessageInterface() {}

bool HttpDynamicMessageInterface::Initialise(StructuredDataI &data) {
  return HttpMessageInterface::Initialise(data);
}

bool HttpDynamicMessageInterface::GetAsStructuredData(
    StreamStructuredDataI &data, HttpProtocol &protocol) {
  bool ok = HttpDataExportI::GetAsStructuredData(data, protocol);
  StreamStructuredData<JsonPrinter> *sdata;
  if (ok) {
    sdata = dynamic_cast<StreamStructuredData<JsonPrinter> *>(&data);
    ok = (sdata != NULL_PTR(StreamStructuredData<JsonPrinter> *));
  }
  if (ok) {
    ok = sdata->GetPrinter()->PrintBegin();
  }
  if (ok) {
    ok = SendDynamicMessage(protocol);
    ok = data.Write("OK", ok ? 1u : 0u);
  }
  if (ok) {
    ok = sdata->GetPrinter()->PrintEnd();
  }
  return ok;
}

bool HttpDynamicMessageInterface::GetAsText(StreamI &stream,
                                            HttpProtocol &protocol) {
  bool ok = HttpDataExportI::GetAsText(stream, protocol);
  if (ok) {
    ok = SendDynamicMessage(protocol);
  }
  return ok;
}

/**
 * @brief Sends the message encoded in the "msg" HTTP command.
 * @details Customize the message to be sent to
 * @param[in] protocol expected to contain a \a msg parameter where to read the
 * name of the message to send.
 * @return true if the message can be sent (only if the "msg" HTTP command was
 * set).
 */
bool HttpDynamicMessageInterface::SendMessage(ReferenceT<Message> &msg) {
  bool ok = msg.IsValid();
  if (ok) {
    msg->SetAsReply(false);
    ErrorManagement::ErrorType err;
    if (msg->ExpectsReply()) {
      err = MessageI::SendMessageAndWaitReply(msg, this);
    } else {
      err = MessageI::SendMessage(msg, this);
    }
    if (err != ErrorManagement::NoError) {
      ok = false;
      REPORT_ERROR_STATIC(ErrorManagement::FatalError,
                          "Could not send message");
    }
  } else {
    REPORT_ERROR_STATIC(ErrorManagement::FatalError, "Message is not valid");
    ok = false;
  }
  return ok;
}

bool HttpDynamicMessageInterface::SendDynamicMessage(HttpProtocol &protocol) {
  bool ok = true;
  ConfigurationDatabase cdb;
  ok &= cdb.CreateRelative("+Parameters");
  ok &= cdb.Write("Class", "ConfigurationDatabase");
  ok &= cdb.MoveToAncestor(1u);
  protocol.MoveAbsolute("InputCommands");
  uint32 num = protocol.GetNumberOfChildren();
  for (uint32 i = 0; i < num; i++) {
    const char *name = protocol.GetChildName(i);
    if (!StringHelper::Compare(name, "destination")) {
      StreamString destination;
      protocol.Read(name, destination);
      cdb.Write("Destination", destination);
    } else if (!StringHelper::Compare(name, "function")) {
      StreamString function;
      protocol.Read(name, function);
      cdb.Write("Function", function);
    } else if (!StringHelper::Compare(name, "mode")) {
      StreamString mode;
      ok &= protocol.Read(name, mode);
      cdb.Write("Mode", mode);
    } else if (!StringHelper::Compare(name, "timeout")) {
      uint32 timeout;
      ok &= protocol.Read(name, timeout);
      cdb.Write("ReplayTimeout", timeout);
    } else if (!StringHelper::Compare(name, "OutputOptions") ||
               !StringHelper::Compare(name, "Peer") ||
               !StringHelper::Compare(name, "InputOptions")) {
      continue;
    } else {
      ok &= cdb.MoveRelative("+Parameters");
      StreamString s;
      ok &= protocol.Read(name, s);
      ok &= cdb.Write(name, s);
      ok &= cdb.MoveToAncestor(1u);
    }
  }
  if (ok) {
    ReferenceT<Message> msg = new Message();
    if (!msg->Initialise(cdb)) {
      REPORT_ERROR(ErrorManagement::RecoverableError,
                   "error in the initialisation of the message");
      msg.RemoveReference();
      return false;
    }
    ok &= SendMessage(msg);
  }
  return ok;
}

CLASS_REGISTER(HttpDynamicMessageInterface, "1.0")
} /* namespace MARTe */
