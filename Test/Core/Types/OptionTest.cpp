#include "OptionTest.h"

#include "Option.h"
#include "TestMacros.h"

struct test_t {
  int x;
  int y;
};

bool OptionTest::TestConstructor() {
  MARTe::Option<bool> a;
  T_ASSERT_TRUE(!a);
  T_ASSERT_TRUE(a.empty());
  // T_ASSERT_THROW(a.val(), ect::Error);
  MARTe::Option<int> b(1);
  T_ASSERT_FALSE(!b);
  T_ASSERT_FALSE(b.empty());
  T_ASSERT_EQ(~b, 1);
  T_ASSERT_EQ(b.val(), 1);
  return true;
}

bool OptionTest::TestOperators() {

  MARTe::Option<MARTe::int64> f(12);
  T_ASSERT_EQ(~f, 12);
  T_ASSERT_FALSE(!f);
  test_t t;
  t.x = 10;
  t.y = 16;
  MARTe::Option<test_t> res(t);
  test_t r;
  r = ~res;
  T_ASSERT_EQ(r.x, t.x);
  T_ASSERT_EQ(r.y, t.y);
  T_ASSERT_FALSE(!res);

  return true;
}

bool OptionTest::TestAssign() {
  MARTe::Option<int> f(12);
  f = MARTe::Option<int>();
  T_ASSERT_TRUE(!f);

  f = 10;
  T_ASSERT_FALSE(!f);
  T_ASSERT_EQ(f.val(), 10);

  f = MARTe::Option<int>(12);
  T_ASSERT_FALSE(!f);
  T_ASSERT_EQ(f.val(), 12);

  return true;
}

int fact(int n) {
  if (n <= 1) {
    return 1;
  }
  return n * fact(n - 1);
}

bool OptionTest::TestMaybe() {
  MARTe::Option<int> x;
  T_ASSERT_TRUE(x.empty());
  MARTe::Option<int> y = MARTe::Option<int>::maybe(fact, x);
  T_ASSERT_TRUE(y.empty());
  x = 10;
  T_ASSERT_FALSE(x.empty());
  y = MARTe::Option<int>::maybe(fact, x);
  T_ASSERT_FALSE(y.empty());
  T_ASSERT_EQ(y.val(), fact(x.val()));
  return true;
}
