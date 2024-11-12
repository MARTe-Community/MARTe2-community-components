#include "RcTest.h"

#include "Rc.h"
#include "TestMacros.h"
#include <error.h>

#define LOG(args...)                                                           \
  {                                                                            \
    printf("LOG [%s:%d] > ", __FILE__, __LINE__);                              \
    printf(args);                                                              \
  }

#define STR(x) #x
#define ESTR(x) STR(#x)

using namespace MARTe;

template <typename T> class RcFriend : public Rc<T> {
public:
  inline RcFriend() : Rc<T>() {}
  inline RcFriend(T *p) : Rc<T>(p) {}
  inline RcFriend(T v) : Rc<T>(v) {}
  inline MARTe::uint32 &refCounter() { return this->GetRefCounter(); }
};

bool PtrTest::TestConstructor() {
  Rc<int> p0;
  T_ASSERT_TRUE(p0.isNull());
  Rc<int> p1(10);
  T_ASSERT_FALSE(p1.isNull());
  Rc<int> p2(new int(10));
  T_ASSERT_FALSE(p2.isNull());
  Rc<int> p3(p0);
  T_ASSERT_TRUE(p3.isNull());
  Rc<int> p4(p1);
  T_ASSERT_FALSE(p4.isNull());
  return true;
}

bool PtrTest::TestDestructor() {
  const int ans = 42;
  int *test = new int(ans);
  T_ASSERT_EQ(*test, ans);
  {
    Rc<int> p(test);
    T_ASSERT_FALSE(p.isNull());
    T_ASSERT_EQ(p.val(), ans);
    T_ASSERT_EQ(*test, ans);
  }
  test = new int(ans);
  {
    Rc<int> p(test);
    T_ASSERT_FALSE(p.isNull());
    T_ASSERT_EQ(*p, ans);
    {
      Rc<int> d(p);
      T_ASSERT_FALSE(p.isNull());
      T_ASSERT_EQ(*p, ans);
      T_ASSERT_FALSE(d.isNull());
      T_ASSERT_EQ(*d, *p);
    }
    T_ASSERT_FALSE(p.isNull());
    T_ASSERT_EQ(*p, ans);
  }
  return true;
}

bool PtrTest::TestAssign() {
  const int ans = 42;
  Rc<int> p(ans);
  T_ASSERT_FALSE(p.isNull());
  T_ASSERT_FALSE(!p);
  T_ASSERT_TRUE(p);
  T_ASSERT_EQ(p.val(), ans);
  p = 14;
  T_ASSERT_EQ(*p, 14);
  int x = p.val();
  T_ASSERT_EQ(x, 14);
  return true;
}

bool PtrTest::TestNull() {
  Rc<int> p;
  T_ASSERT_TRUE(p.isNull());
  p = Rc<int>(5);
  T_ASSERT_FALSE(p.isNull());
  T_ASSERT_EQ(p, 5);
  T_ASSERT_DE(p, 6);
  p.del();
  T_ASSERT_DE(p, 5);
  T_ASSERT_TRUE(p.isNull());
  T_ASSERT_FALSE(p);
  // T_ASSERT_THROW(p.val(), Error);
  T_ASSERT_FALSE(p == 5);
  p = 6;
  T_ASSERT_TRUE(p);
  T_ASSERT_EQ(p, 6);
  return true;
}

struct test_t {
  int x;
};

bool PtrTest::TestOperators() {
  const int ans = 42;
  int *a = new int(ans);
  Rc<int> p;
  T_ASSERT_TRUE(!p);
  T_ASSERT_DE(p, a);
  T_ASSERT_EQ(p, NULL_PTR(int *));
  Rc<int> b;
  T_ASSERT_EQ(p, b);
  b = Rc<int>(ans);
  T_ASSERT_TRUE(b);
  T_ASSERT_DE(p, b);

  Rc<test_t> x;
  // T_ASSERT_THROW(x->x = 1, Error);
  test_t c;
  c.x = ans;
  x = Rc<test_t>(c);
  T_ASSERT_TRUE(x);
  T_ASSERT_EQ(x->x, ans);
  delete a;
  return true;
}

struct Ref {
  RcFriend<Ref> next;
  RcFriend<Ref> prev;

  Ref(RcFriend<Ref> prev = NULL_PTR(Ref *)) {
    this->next = RcFriend<Ref>();
    this->prev %= prev; // weak reference
  }
  ~Ref() {
    next.del();
    prev.del();
  }
};

bool PtrTest::TestReferenceCycle() {
  MARTe::uint32 *a_counter = NULL_PTR(MARTe::uint32 *);
  MARTe::uint32 *b_counter = NULL_PTR(MARTe::uint32 *);
  {
    Ref *ref = new Ref();
    RcFriend<Ref> a = RcFriend<Ref>(ref);
    a_counter = &a.refCounter();
    T_ASSERT_EQ(*a_counter, 1);
    T_ASSERT_EQ(b_counter, NULL_PTR(MARTe::uint32 *));
    {
      RcFriend<Ref> b = new Ref(a);
      b_counter = &b.refCounter();
      T_ASSERT_EQ(*a_counter, 1);
      T_ASSERT_EQ(*b_counter, 1);
      a->next = b;
      T_ASSERT_EQ(*a_counter, 1);
      T_ASSERT_EQ(*b_counter, 2);
    }
    T_ASSERT_EQ(*a_counter, 1);
    T_ASSERT_EQ(*b_counter, 1);
  }
  return true;
}

bool PtrTest::TestWeakReferences() {
  Rc<int> a(10);
  T_ASSERT_FALSE(a.isNull())
  T_ASSERT_FALSE(a.isWeak())
  Rc<int> b = ~a;
  Rc<int> c = ~a;
  T_ASSERT_TRUE(b.isWeak())
  T_ASSERT_FALSE(b.isNull())
  T_ASSERT_TRUE(c.isWeak())
  T_ASSERT_FALSE(c.isNull())
  a.del();
  T_ASSERT_TRUE(a.isNull())
  T_ASSERT_TRUE(b.isNull())
  T_ASSERT_TRUE(c.isNull())
  return true;
}
