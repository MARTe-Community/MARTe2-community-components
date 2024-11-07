#ifndef MESSAGEINTERFACE_TEST_H
#define MESSAGEINTERFACE_TEST_H

class MessageInterfaceTester {
public:
  bool TestInit();
  bool TestEmptyMessage();
  bool TestTextReplay();
  bool TestStructuredReplay();
};
#endif
