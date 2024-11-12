#include "MessageInterfaceTest.h"
#include "gtest/gtest.h"

TEST(MessageInterface, TestInitialization) {
  MessageInterfaceTester tester;
  ASSERT_TRUE(tester.TestInit());
}

TEST(MessageInterface, TestEmptyMessage) {
  MessageInterfaceTester tester;
  ASSERT_TRUE(tester.TestEmptyMessage());
}

TEST(MessageInterface, TestTextReplay) {
  MessageInterfaceTester tester;
  ASSERT_TRUE(tester.TestTextReplay());
}

TEST(MessageInterface, TestStructuedReplay) {
  MessageInterfaceTester tester;
  ASSERT_TRUE(tester.TestStructuredReplay());
}
