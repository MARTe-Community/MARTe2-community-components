#include "gtest/gtest.h"

#include "ResultTest.h"

TEST(Result, Constuctor) {
  ResultTest tester;
  ASSERT_TRUE(tester.TestConstructor());
}

TEST(Result, Succeded) {
  ResultTest tester;
  ASSERT_TRUE(tester.TestSucceded());
}


TEST(Result, Operators) {
  ResultTest tester;
  ASSERT_TRUE(tester.TestOperators());
}
