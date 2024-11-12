#include "gtest/gtest.h"
#include "RcTest.h"

TEST(Ptr, Constructor) {
  PtrTest tester;
  ASSERT_TRUE(tester.TestConstructor());
}

TEST(Ptr, Destructor) {
  PtrTest tester;
  ASSERT_TRUE(tester.TestDestructor());
}

TEST(Ptr, Assign) {
  PtrTest tester;
  ASSERT_TRUE(tester.TestAssign());
}

TEST(Ptr, Null) {
  PtrTest tester;
  ASSERT_TRUE(tester.TestNull());
}

TEST(Ptr, Operator) {
  PtrTest tester;
  ASSERT_TRUE(tester.TestOperators());
}

TEST(Ptr, ReferenceCycle) {
  PtrTest tester;
  ASSERT_TRUE(tester.TestReferenceCycle());
}

TEST(Ptr, WeakReferences) {
  PtrTest tester;
  ASSERT_TRUE(tester.TestWeakReferences());
}
