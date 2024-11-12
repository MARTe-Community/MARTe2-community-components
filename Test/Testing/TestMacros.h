#ifndef _T_TEST_MACROS_H__
#define _T_TEST_MACROS_H__

#include <stdio.h>
#include <string.h>

#define T_LOG(fmt, ...)                                                        \
  printf("# LOG > ");                                                          \
  printf(fmt, __VA_ARGS__);                                                    \
  printf("\n");

#define T_FAIL(REASON)                                                         \
  {                                                                            \
    printf("%s failed at %s:%d\n", __FUNCTION__, __FILE__, __LINE__);          \
    printf("  > %s\n", REASON);                                                \
    return false;                                                              \
  }
#define T_FAIL_MSG(fmt, ...)                                                   \
  {                                                                            \
    printf("%s failed at %s:%d\n", __FUNCTION__, __FILE__, __LINE__);          \
    printf("  > " fmt "\n", __VA_ARGS__);                                      \
    return false;                                                              \
  }

#define T_ASSERT_EQ(a, b)                                                      \
  if (!(a == b)) {                                                             \
    T_FAIL(#a " is not equal to " #b);                                         \
  }

#define T_ASSERT_DE(a, b)                                                      \
  if (!(a != b))                                                               \
    T_FAIL(#a " is equal to " #b);

#define T_ASSERT_LT(a, b)                                                      \
  if (!(a < b))                                                                \
    T_FAIL(#a " is not less then " #b);

#define T_ASSERT_LTE(a, b)                                                     \
  if (!(a <= b))                                                               \
    T_FAIL(#a " is not less or equal to " #b);

#define T_ASSERT_GT(a, b)                                                      \
  if (!(a > b))                                                                \
    T_FAIL(#a " is not greater then " #b);

#define T_ASSERT_GTE(a, b)                                                     \
  if (!(a >= b)) {                                                             \
    T_FAIL(#a " is not greater or equal to " #b);                              \
  }

#define T_ASSERT_NEAR(a, b, th)                                                \
  if ((a - b) * (a - b) > th * th) {                                           \
    T_FAIL(#a " is not close to " #b);                                         \
  }

#define T_ASSERT_NEAR_REL(a, b, th)                                            \
  if (((a - b) * (a - b) / (b * b)) > (th * th)) {                             \
    T_FAIL(#a " is not close to " #b);                                         \
  }

#define T_ASSERT_TRUE(a)                                                       \
  if (!(a)) {                                                                  \
    T_FAIL("expression " #a " is false");                                      \
  }

#define T_ASSERT_FALSE(a)                                                      \
  if (a) {                                                                     \
    T_FAIL("expression " #a " is true");                                       \
  }

#define T_ASSERT_THROW(a, err_t)                                               \
  {                                                                            \
    bool ok = false;                                                           \
    try {                                                                      \
      a;                                                                       \
    } catch (err_t & e) {                                                      \
      ok = true;                                                               \
    } catch (...) {                                                            \
      T_FAIL("different exception thrown by `" #a "`, expected: " #err_t);     \
    }                                                                          \
    if (!ok)                                                                   \
      T_FAIL("no exception thrown by " #a);                                    \
  }

#define T_ASSERT_STREQ(a, b)                                                   \
  if (strcmp(a, b) != 0)                                                       \
  T_FAIL_MSG("string `%s` different from string `%s`", a, b)

#define T_ASSERT_STRNE(a, b)                                                   \
  if (strcmp(a, b) == 0)                                                       \
  T_FAIL_MSG("string `%s` equal to string `%s`", a, b)

#define T_ASSERT_NO_THROW(exp)                                                 \
  {                                                                            \
    try {                                                                      \
      exp;                                                                     \
    } catch (const ect::Error &e) {                                            \
      T_FAIL_MSG("expression " #exp " has thrown an exception: %s", e.what()); \
    } catch (...) {                                                            \
      T_FAIL("expression " #exp " has thrown an exception");                   \
    }                                                                          \
  }

#define T_ASSERT_SUCC(res)                                                     \
  {                                                                            \
    if (!res) {                                                                \
      printf(" > Failed, error message: %s\n", res.err().what());              \
      T_FAIL(#res " resulted in an error");                                    \
    }                                                                          \
  }

#define T_ASSERT_FAIL(res)                                                     \
  {                                                                            \
    if (res.succeded()) {                                                      \
      T_FAIL(#res " is not an error");                                         \
    }                                                                          \
  }

#endif
