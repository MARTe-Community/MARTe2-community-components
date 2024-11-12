#include "gtest/gtest.h"

#include "StrTest.h"

TEST(Str, TestConstructor) {
  StrTest tester;
  ASSERT_TRUE(tester.TestConstructor());
}

TEST(Str, TestAddChar) {
  StrTest tester;
  ASSERT_TRUE(tester.TestAddChar());
}

TEST(Str, TestAddString) {
  StrTest tester;
  ASSERT_TRUE(tester.TestAddString());
}

TEST(Str, TestAssignOp) {
  StrTest tester;
  ASSERT_TRUE(tester.TestAssignOperator());
}

TEST(Str, TestEqualityOp) {
  StrTest tester;
  ASSERT_TRUE(tester.TestEqualityOperator());
}

TEST(Str, TestDisequalityOp) {
  StrTest tester;
  ASSERT_TRUE(tester.TestDisequalityOperator());
}

TEST(Str, TestSortingOps) {
  StrTest tester;
  ASSERT_TRUE(tester.TestSortingOperators());
}

TEST(Str, TestAtOp) {
  StrTest tester;
  ASSERT_TRUE(tester.TestAtOperator());
}

TEST(Str, TestClear) {
  StrTest tester;
  ASSERT_TRUE(tester.TestClear());
}

TEST(Str, TestMaxSize) {
  StrTest tester;
  ASSERT_TRUE(tester.TestMaxSize());
}

TEST(Str, TestFind) {
  StrTest tester;
  ASSERT_TRUE(tester.TestFind());
}

TEST(Str, TestSubstrSimple) {
  StrTest tester;
  ASSERT_TRUE(tester.TestSubstringSimple());
}

TEST(Str, TestSubstrFull) {
  StrTest tester;
  ASSERT_TRUE(tester.TestSubstringFull());
}

TEST(Str, TestDifferentEOL) {
  StrTest tester;
  ASSERT_TRUE(tester.TestDifferentEOL());
}

TEST(Str, TestHash) {
  StrTest tester;
  ASSERT_TRUE(tester.TestHash());
}
