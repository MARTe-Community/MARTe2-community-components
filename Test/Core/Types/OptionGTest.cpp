#include "OptionTest.h"
#include "gtest/gtest.h"

TEST(optional, constructor) {
  OptionTest tester;
  ASSERT_TRUE(tester.TestConstructor());
}

TEST(optional, operators) {
  OptionTest tester;
  ASSERT_TRUE(tester.TestOperators());
}

TEST(optional, assign) {
  OptionTest tester;
  ASSERT_TRUE(tester.TestAssign());
}

TEST(optional, maybe) {
  OptionTest tester;
  ASSERT_TRUE(tester.TestMaybe());
}
