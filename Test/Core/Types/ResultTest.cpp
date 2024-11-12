#include "ErrorType.h"
#include "ResultTest.h"
#include "TestMacros.h"

#include "Result.h"

struct simple_t {
  int a;
  int b;
};

using namespace MARTe;

bool ResultTest::TestConstructor() {
  Result<bool> a = Result<bool>::Succ(true);
  T_ASSERT_TRUE(a.val());
  Result<bool, int> b = Result<bool, int>::Fail(42);
  T_ASSERT_EQ(b.err(), 42);
  Result<int, int> c = Result<int, int>::Succ(10);
  T_ASSERT_EQ(c.val(), 10);
  Result<float> d = Result<float>::Succ(0.1);
  T_ASSERT_NEAR(d.val(), 0.1, 1e-6);
  Result<const char *> e = "ciao";
  T_ASSERT_STREQ(e.val(), "ciao");
  return true;
}

bool ResultTest::TestSucceded() {
  Result<short> a = Result<short>::Succ(120);
  T_ASSERT_TRUE(a.succeded());
  T_ASSERT_FALSE(!a);
  Result<double> b = Result<double>::Fail(ErrorManagement::NoError);
  T_ASSERT_FALSE(b.succeded());
  T_ASSERT_TRUE(!b);
  return true;
}

bool ResultTest::TestOperators() {
  int set_a = 10;
  int set_b = 42;
  simple_t ref;
  ref.a = set_a;
  ref.b = set_b;
  Result<simple_t> a = Result<simple_t>::Succ(ref);
  simple_t val = +a;
  T_ASSERT_EQ(val.a, set_a);
  T_ASSERT_EQ(val.b, set_b);

  Result<simple_t> b = Result<simple_t>::Fail(ErrorManagement::FatalError);
  ErrorManagement::ErrorType e = -b;
  T_ASSERT_EQ(e, ErrorManagement::FatalError);
  Result<short> x = Result<short>::Fail(ErrorManagement::Timeout);
  T_ASSERT_TRUE(!x);
  x = Result<short>::Succ(50);

  T_ASSERT_FALSE(!x);
  T_ASSERT_EQ(x.val(), 50);
  return true;
}
