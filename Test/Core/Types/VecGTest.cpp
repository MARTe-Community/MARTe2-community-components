#include "VecTest.h"
#include "gtest/gtest.h"

TEST(Vec, TestConstructor) {
  VecTest tester;
  ASSERT_TRUE(tester.TestConstructor());
}

TEST(Vec, TestAt) {
  VecTest tester;
  ASSERT_TRUE(tester.TestAt());
}

TEST(Vec, TestAdd) {
  VecTest tester;
  ASSERT_TRUE(tester.TestAdd());
}

TEST(Vec, TestAssign) {
  VecTest tester;
  ASSERT_TRUE(tester.TestAssign());
}

TEST(Vec, TestRemove) {
  VecTest tester;
  ASSERT_TRUE(tester.TestRemove());
}

TEST(Vec, TestFind) {
  VecTest tester;
  ASSERT_TRUE(tester.TestFind());
}

TEST(Vec, TestEquality) {
  VecTest tester;
  ASSERT_TRUE(tester.TestEquality());

}

TEST(Vec, TestSet) {
  VecTest tester;
  ASSERT_TRUE(tester.TestSet());
}


