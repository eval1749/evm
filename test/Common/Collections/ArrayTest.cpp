#include "precomp.h"
// @(#)$Id$
//
// Evita String Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common.h"
#include "Common/Collections.h"

namespace CommonTest {

using namespace Common;
using namespace Common::Collections;

class ArrayTest : public ::testing::Test {
  protected: Array_<int> arr_;

  protected: ArrayTest() : arr_(3) {
    arr_.Set(0, 'A');
    arr_.Set(1, 'B');
    arr_.Set(2, 'C');
  }
};

TEST_F(ArrayTest, Copy) {
  Array_<int> a1(3);
  a1.Set(0, 'A');
  a1.Set(1, 'B');
  a1.Set(2, 'C');

  Array_<int> a2(a1);
  EXPECT_EQ('A', a2.Get(0));
  EXPECT_EQ('B', a2.Get(1));
  EXPECT_EQ('C', a2.Get(2));
  EXPECT_TRUE(a1 == a2);

  int const v[] = { 'X', 'Y', 'Z' };
  Array_<int> a3(v, ARRAYSIZE(v));
  EXPECT_EQ('X', a3.Get(0));
  EXPECT_EQ('Y', a3.Get(1));
  EXPECT_EQ('Z', a3.Get(2));
  EXPECT_TRUE(a1 != a3);
  EXPECT_FALSE(a1 == a3);

  a1 = a3;
  EXPECT_TRUE(a1 == a3);
  EXPECT_FALSE(a1 != a3);
}

TEST_F(ArrayTest, Enum) {
  char const sz[] = "qwerty";
  Array_<char> a(sz, ::lstrlenA(sz));
  Array_<char>::Enumerator en(a);
  auto i = 0;
  foreach (Enum_<char>, chars, en) {
    EXPECT_EQ(sz[i], *chars);
    i++;
  }
}

#if 0
TEST_F(ArrayTest, Equals) {
  Array_<Int32> a1(3);
  EXPECT_EQ(3, a1.Count());
  a1.Set(0, 'A');
  a1.Set(1, 'B');
  a1.Set(2, 'C');

  Array_<Int32> a2(&a1);
  EXPECT_EQ(3, a2.Count());
  EXPECT_TRUE(a2.Equals(&a1));
}
#endif

TEST_F(ArrayTest, Equals2) {
  Array_<int> a(3);
  a.Set(0, 1);
  a.Set(1, 2);
  a.Set(2, 3);

  Array_<int> b(3);
  b.Set(0, 1);
  b.Set(1, 2);
  b.Set(2, 3);

  EXPECT_EQ(a, b);

  Array_<int> c(0);
  EXPECT_NE(a, c);
}

TEST_F(ArrayTest, Sort) {
  class Local {
    public: static int LibCallback Compare(char const a, char const b) {
      return a - b;
    }
  };

  char const sz[] = "qwerty";
  Array_<char> a(sz, ::lstrlenA(sz));
  a.Sort(Local::Compare);
  EXPECT_EQ('e', a.Get(0));
  EXPECT_EQ('q', a.Get(1));
  EXPECT_EQ('r', a.Get(2));
  EXPECT_EQ('t', a.Get(3));
  EXPECT_EQ('w', a.Get(4));
  EXPECT_EQ('y', a.Get(5));
}


TEST_F(ArrayTest, ToString) {
  EXPECT_EQ(String("[65, 66, 67]"), arr_.ToString());
}
}  // CommonTest
