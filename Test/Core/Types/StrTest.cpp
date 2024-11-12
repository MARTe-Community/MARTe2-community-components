#include "Option.h"
#include "StrTest.h"
#include "TestMacros.h"

#include "Str.h"

bool StrTest::TestConstructor() {
  MARTe::Str a;
  T_ASSERT_EQ(a.len(), 0);
  T_ASSERT_STREQ(a.cstr(), "");
  MARTe::Str b("hello");
  T_ASSERT_EQ(b.len(), 5);
  T_ASSERT_STREQ(b.cstr(), "hello");
  MARTe::Str c(b);
  T_ASSERT_EQ(c.len(), b.len());
  T_ASSERT_STREQ(c.cstr(), b.cstr());
  return true;
}

bool StrTest::TestAddChar() {
  const char *str = "ciao";
  MARTe::Str s;
  for (int i = 0; i < 4; i++) {
    s += str[i];
  }
  T_ASSERT_EQ(s.len(), 4);
  T_ASSERT_STREQ(s.cstr(), str);
  return true;
}

bool StrTest::TestAddString() {
  MARTe::Str a = "hello";
  MARTe::Str b = "world";
  MARTe::Str c = a + b;

  T_ASSERT_STREQ(a.cstr(), "hello");
  T_ASSERT_STREQ(b.cstr(), "world");
  T_ASSERT_STREQ((a + b).cstr(), "helloworld");
  T_ASSERT_STREQ(c.cstr(), "helloworld");
  T_ASSERT_STREQ((a + "world").cstr(), "helloworld");
  T_ASSERT_STREQ((b + 's').cstr(), "worlds");

  char longString[252];
  memset(&longString, 'x', 251);
  longString[251] = 0;
  b = longString;
  T_ASSERT_EQ(b.len(), 251);
  T_ASSERT_EQ(b.len(), strlen(longString));
  T_ASSERT_EQ((a + longString).len(), (a + b).len());
  T_ASSERT_STREQ((a + longString).cstr(), (a + b).cstr());
  return true;
}

bool StrTest::TestAssignOperator() {
  MARTe::Str a = "hello";
  T_ASSERT_STREQ(a.cstr(), "hello");
  MARTe::Str b = "ciao";
  T_ASSERT_STREQ(b.cstr(), "ciao");
  T_ASSERT_EQ(a.len(), 5);
  a = b;
  T_ASSERT_EQ(a.len(), 4);
  T_ASSERT_STREQ(a.cstr(), "ciao");
  return true;
}

bool StrTest::TestEqualityOperator() {
  MARTe::Str a = "hello";
  T_ASSERT_TRUE(a == "hello");
  T_ASSERT_FALSE(a == "ciao");
  T_ASSERT_FALSE(a == "bello");
  MARTe::Str b = "hello";
  T_ASSERT_TRUE(a == b);
  b = "ciao";
  T_ASSERT_FALSE(a == b);
  b = "bello";
  T_ASSERT_FALSE(a == b);
  return true;
}

bool StrTest::TestDisequalityOperator() {
  MARTe::Str a = "hello";
  T_ASSERT_FALSE(a != "hello");
  T_ASSERT_TRUE(a != "ciao");
  T_ASSERT_TRUE(a != "bello");
  MARTe::Str b = "ciao";
  T_ASSERT_TRUE(a != b);
  b = "hello";
  T_ASSERT_FALSE(a != b);
  b = "miaos";
  T_ASSERT_TRUE(a != b);
  return true;
}

bool StrTest::TestSortingOperators() {
  MARTe::Str a = "";
  MARTe::Str b = "";
  T_ASSERT_FALSE(a > b);
  T_ASSERT_FALSE(a < b);
  a = "a";
  T_ASSERT_TRUE(a > b);
  T_ASSERT_FALSE(a < b);
  b = "a";
  T_ASSERT_FALSE(a > b);
  T_ASSERT_FALSE(a < b);
  a = "aa";
  T_ASSERT_TRUE(a > b);
  T_ASSERT_FALSE(a < b);
  a = "b";
  T_ASSERT_TRUE(a > b);
  T_ASSERT_FALSE(a < b);
  a = "A";
  T_ASSERT_TRUE(a < b);
  T_ASSERT_FALSE(a > b);
  a = "0";
  T_ASSERT_TRUE(a < b);
  T_ASSERT_FALSE(a > b);
  return true;
}

bool StrTest::TestAtOperator() {
  MARTe::Str a = "hello";
  T_ASSERT_TRUE(a[0] == 'h');
  T_ASSERT_TRUE(a[4] == 'o');
  T_ASSERT_TRUE(a[-1] == 'o');
  a.set(0, 'b');
  T_ASSERT_TRUE(a[0] == 'b');
  T_ASSERT_STREQ(a.cstr(), "bello");
  return true;
}

bool StrTest::TestClear() {
  MARTe::Str a = "hello";
  T_ASSERT_EQ(a.len(), 5);
  T_ASSERT_STREQ(a.cstr(), "hello");
  a.clear();
  T_ASSERT_EQ(a.len(), 0);
  T_ASSERT_STREQ(a.cstr(), "");
  return true;
}

bool StrTest::TestDifferentEOL() {
  const char *str = ":0.500\r";
  MARTe::Str a = str;
  T_ASSERT_STREQ(a.cstr(), str);
  T_ASSERT_EQ(a.len(), strlen(str));
  a += 0;
  T_ASSERT_EQ(a.len(), strlen(str));
  return true;
}

bool StrTest::TestMaxSize() {
  MARTe::Str a;
  for (MARTe::uint32 i = 0; i < 1024u; i++) {
    a += '+';
    T_ASSERT_EQ(a.len(), i + 1);
  }
  a += ' ';
  return true;
}

bool StrTest::TestFind() {
  MARTe::Str str = "hello world!";
  MARTe::Option<MARTe::uint32> res = str.find("hello");
  T_ASSERT_FALSE(res.empty());
  T_ASSERT_EQ(res.val(), 0);
  res = str.find("hello", 1);
  T_ASSERT_TRUE(res.empty());
  res = str.find("ello", 1);
  T_ASSERT_FALSE(res.empty());
  T_ASSERT_EQ(res.val(), 1);
  res = str.find("world");
  T_ASSERT_FALSE(res.empty());
  T_ASSERT_EQ(res.val(), 6);
  res = str.find("world?");
  T_ASSERT_TRUE(res.empty());
  res = str.find("hello world!xxx");
  T_ASSERT_TRUE(res.empty());
  str = "($ANS)";
  res = str.find("$ANS");
  T_ASSERT_FALSE(res.empty());
  T_ASSERT_EQ(res.val(), 1);
  return true;
}

bool StrTest::TestSubstringSimple() {
  MARTe::Str str = "hello world";
  T_ASSERT_EQ(str.len(), 11);
  T_ASSERT_STREQ(str.cstr(), "hello world");
  MARTe::Str r_str = str.substr(5);
  T_ASSERT_EQ(r_str.len(), 5);
  T_ASSERT_STREQ(r_str.cstr(), "hello");
  r_str = str.substr(-5);
  T_ASSERT_EQ(r_str.len(), 6);
  T_ASSERT_STREQ(r_str.cstr(), "hello ");
  r_str = str.substr(12);
  T_ASSERT_EQ(r_str.len(), str.len());
  r_str = str.substr(-12);
  T_ASSERT_EQ(r_str.len(), 0);
  return true;
}

bool StrTest::TestSubstringFull() {
  MARTe::Str str = "hello world";
  T_ASSERT_EQ(str.len(), 11);
  T_ASSERT_STREQ(str.cstr(), "hello world");
  MARTe::Str r_str = str.substr(1, 5);
  T_ASSERT_EQ(r_str.len(), 4);
  T_ASSERT_STREQ(r_str.cstr(), "ello");
  r_str = str.substr(5, 0);
  T_ASSERT_EQ(r_str.len(), 6);
  T_ASSERT_STREQ(r_str.cstr(), " world");

  r_str = str.substr(11, 12);
  T_ASSERT_EQ(r_str.len(), 0);
  r_str = str.substr(5, 4);
  T_ASSERT_EQ(r_str.len(), 0);
  r_str = str.substr(5, 16);
  T_ASSERT_EQ(r_str.len(), str.len() - 5);
  r_str = str.substr(13, -1);
  T_ASSERT_EQ(r_str.len(), 0);

  str = "constant::0.0";
  r_str = str.substr(10, 0);
  T_ASSERT_STREQ(r_str.cstr(), "0.0");
  return true;
}

bool StrTest::TestHash() {
  MARTe::Str a = "ciao";
  MARTe::Str b = "maio";
  T_ASSERT_DE(a.hash(), b.hash());
  b = "hello";
  T_ASSERT_DE(a.hash(), b.hash());
  a = "hola";
  T_ASSERT_DE(a.hash(), b.hash());
  return true;
}
