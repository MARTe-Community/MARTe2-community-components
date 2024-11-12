#include "VecTest.h"
#include "Option.h"
#include "Vec.h"
#include "TestMacros.h"

bool VecTest::TestConstructor() {
  MARTe::Vec<int> a;
  T_ASSERT_EQ(a.len(), 0);
  T_ASSERT_EQ(a.mem_size(), 16);

  MARTe::Vec<float> b(128);
  T_ASSERT_EQ(b.len(), 0);
  T_ASSERT_EQ(b.mem_size(), 128);

  MARTe::Vec<char> c("123", 3);
  T_ASSERT_EQ(c.len(), 3);
  T_ASSERT_EQ(c.mem_size(), 19);
  for (MARTe::uint32 i = 0; i < c.len(); i++) {
    T_ASSERT_EQ(c[i], "123"[i]);
  }

  MARTe::Vec<char> d(c);
  T_ASSERT_EQ(c.len(), d.len());
  T_ASSERT_DE(c.mem_size(), d.mem_size());
  for (MARTe::uint32 i = 0; i < c.len(); i++) {
    T_ASSERT_EQ(c[i], d[i]);
  }
  return true;
}

bool VecTest::TestAt() {
  MARTe::Vec<int> a;
  T_ASSERT_EQ(a.len(), 0);
  // T_ASSERT_THROW(a[0], ect::ExtendedError);
  T_ASSERT_TRUE(!a.at(0));

  double arr[] = {1.0, 2.0, 3.0};

  MARTe::Vec<double> b(arr, 3);
  T_ASSERT_EQ(b.len(), 3);
  for (int i = 0; i < 3; i++) {
    T_ASSERT_FALSE(!b.at(i));
    T_ASSERT_NEAR(b.at(i).val(), arr[i], 1e-6);
    T_ASSERT_NEAR(b[i], arr[i], 1e-6);
  }
  T_ASSERT_FALSE(!b.at(-1));
  T_ASSERT_NEAR(b.at(-1).val(), arr[2], 1e-6);
  T_ASSERT_NEAR(b[-1], arr[2], 1e-6);
  b[1] = 10.0;
  T_ASSERT_NEAR(b.at(1).val(), 10.0, 1e-6);
  T_ASSERT_NEAR(b[1], 10.0, 1e-6);
  T_ASSERT_TRUE(!b.at(-5));
  // T_ASSERT_THROW(b[-5], ect::ExtendedError);
  return true;
}

bool VecTest::TestAdd() {
  MARTe::Vec<int> a;
  a.append(42);
  T_ASSERT_EQ(a.len(), 1);
  T_ASSERT_EQ(a[0], 42);
  a += 68;
  T_ASSERT_EQ(a.len(), 2);
  T_ASSERT_EQ(a[1], 68);
  a.clear();
  T_ASSERT_EQ(a.len(), 0);
  T_ASSERT_EQ(a.mem_size(), 16);
  for (MARTe::uint32 i = 0; i < 32; i++) {
    a += i;
    T_ASSERT_EQ(a.len(), i + 1);
    T_ASSERT_EQ(a[i], (int)i);
  }
  T_ASSERT_EQ(a.mem_size(), 32);
  a.clear();
  a += 18;
  T_ASSERT_EQ(a.len(), 1);
  T_ASSERT_EQ(a.mem_size(), 32);
  T_ASSERT_EQ(a[0], 18);
  a.reduce();
  T_ASSERT_EQ(a.len(), 1);
  T_ASSERT_EQ(a.mem_size(), 16);
  T_ASSERT_EQ(a[0], 18);
  return true;
}

bool VecTest::TestAssign() {
  int array[] = {10, 11, 12, 13};
  MARTe::Vec<int> a;
  MARTe::Vec<int> b(array, 4);
  T_ASSERT_EQ(a.len(), 0);
  T_ASSERT_EQ(b.len(), 4);
  T_ASSERT_EQ(b[0], array[0]);
  T_ASSERT_TRUE(!a.at(0));
  a = b;
  T_ASSERT_EQ(a.len(), b.len());
  for (MARTe::uint32 i = 0; i < a.len(); i++) {
    T_ASSERT_EQ(a[i], array[i]);
  }
  int bigarray[32];
  MARTe::Vec<int> c(bigarray, 32);
  a = c;
  T_ASSERT_EQ(c.mem_size(), 48);
  T_ASSERT_EQ(a.len(), c.len());
  T_ASSERT_EQ(a.mem_size(), c.mem_size());
  return true;
}

bool VecTest::TestRemove() {
  int array[] = {10, 11, 12, 13};
  MARTe::Vec<int> a(array, 4);
  T_ASSERT_EQ(a.len(), 4);
  T_ASSERT_EQ(a[0], 10);
  T_ASSERT_TRUE(a.remove(0));
  T_ASSERT_EQ(a.len(), 3);
  T_ASSERT_EQ(a[0], 11);
  T_ASSERT_EQ(a[2], 13);
  T_ASSERT_TRUE(!a.at(3));
  T_ASSERT_TRUE(a.remove(1));
  T_ASSERT_EQ(a.len(), 2);
  T_ASSERT_EQ(a[0], 11);
  T_ASSERT_EQ(a[1], 13);
  T_ASSERT_TRUE(!a.at(2));
  T_ASSERT_FALSE(a.remove(3));
  return true;
}

bool VecTest::TestFind() {
  int array[] = {10, 11, 12, 13};
  MARTe::Vec<int> a(array, 4);
  MARTe::Option<MARTe::uint32> i = a.find(11);
  T_ASSERT_FALSE(!i);
  T_ASSERT_EQ(~i, 1);
  T_ASSERT_TRUE(a.contains(11));
  T_ASSERT_TRUE(a.remove(1));
  T_ASSERT_FALSE(a.contains(11));
  i = a.find(11);
  T_ASSERT_TRUE(!i);
  // T_ASSERT_THROW(~i, ect::ExtendedError);
  return true;
}

bool VecTest::TestEquality() {
  int array[] = {10, 11, 12, 13};
  MARTe::Vec<int> a(array, 4);
  MARTe::Vec<int> b(array, 3);

  T_ASSERT_FALSE(a == b);
  T_ASSERT_TRUE(a != b);
  b += 5;

  T_ASSERT_FALSE(a == b);
  T_ASSERT_TRUE(a != b);

  b.set(array, 4);
  T_ASSERT_TRUE(a == b);
  T_ASSERT_FALSE(a != b);
  return true;
}

bool VecTest::TestSet() {
  int array[] = {10, 11, 12, 13};
  MARTe::Vec<int> a;
  T_ASSERT_EQ(a.len(), 0);
  T_ASSERT_EQ(a.mem_size(), 16u);
  a.set(array, 4);
  T_ASSERT_EQ(a.len(), 4);
  T_ASSERT_EQ(a.mem_size(), 16u);
  T_ASSERT_EQ(a[0], 10);

  MARTe::Vec<int> b(1);

  T_ASSERT_EQ(b.len(), 0);
  T_ASSERT_EQ(b.mem_size(), 1u);
  b.set(array, 4);
  T_ASSERT_EQ(b.len(), 4);
  T_ASSERT_EQ(b.mem_size(), 20u);
  T_ASSERT_EQ(b[0], 10);

  return true;
}

