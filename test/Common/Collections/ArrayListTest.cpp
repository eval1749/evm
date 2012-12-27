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

class ArrayListTest : public ::testing::Test {
  protected: ArrayList_<int> list_;
  protected: ArrayListTest() {
    list_.Add(1);
    list_.Add(2);
    list_.Add(3);
  }
};

TEST_F(ArrayListTest, Basic) {
  ArrayList_<int> list;
  EXPECT_EQ(0, list.Count());

  list.Add(1);
  list.Add(2);
  list.Add(3);

  EXPECT_EQ(3, list.Count());
  EXPECT_TRUE(list.Contains(1));
  EXPECT_TRUE(list.Contains(2));
  EXPECT_TRUE(list.Contains(3));
  EXPECT_FALSE(list.Contains(4));

  EXPECT_EQ(1, list.Get(0));
  EXPECT_EQ(2, list.Get(1));
  EXPECT_EQ(3, list.Get(2));

  list.Clear();
  EXPECT_EQ(0, list.Count());
  EXPECT_TRUE(list.IsEmpty());
}

TEST_F(ArrayListTest, ctor_Collection) {
  decltype(list_) list(list_);
  EXPECT_EQ(3, list.Count());
  EXPECT_EQ(1, list.Get(0));
  EXPECT_EQ(2, list.Get(1));
  EXPECT_EQ(3, list.Get(2));
}

TEST_F(ArrayListTest, AddAll) {
  decltype(list_) list;
  list.AddAll(list_);
  EXPECT_EQ(3, list.Count());
  EXPECT_EQ(1, list.Get(0));
  EXPECT_EQ(2, list.Get(1));
  EXPECT_EQ(3, list.Get(2));
}

TEST_F(ArrayListTest, Copy) {
  ArrayList_<int> a1;
  a1.Add('A');
  a1.Add('B');

  ArrayList_<int> a2(a1);
  EXPECT_EQ('A', a2.Get(0));
  EXPECT_EQ('B', a2.Get(1));

  ArrayList_<int> a3;
  a3 = a1;
  EXPECT_EQ('A', a3.Get(0));
  EXPECT_EQ('B', a3.Get(1));
}

TEST_F(ArrayListTest, Enum) {
  auto i = 0;
  foreach (ArrayList_<int>::Enum, ints, list_) {
    i++;
    EXPECT_EQ(i, *ints);
  }
}

TEST_F(ArrayListTest, Enumerator) {
  ArrayList_<int>::Enumerator int_enum(list_);
  auto i = 0;
  foreach (Enum_<int>, ints, int_enum) {
    i++;
    EXPECT_EQ(i, *ints);
  }
}

TEST_F(ArrayListTest, Equals) {
  ArrayList_<int> a(3);
  a.Add(1);
  a.Add(2);
  a.Add(3);

  ArrayList_<int> b(3);
  b.Add(1);
  b.Add(2);
  b.Add(3);

  EXPECT_EQ(a, b);
  EXPECT_TRUE(a.Equals(b));

  ArrayList_<int> c(0);
  EXPECT_NE(a, c);
  EXPECT_FALSE(a.Equals(c));
}

TEST_F(ArrayListTest, ForEach) {
  auto index = 0;
  for (auto const value: list_) {
    ++index;
    EXPECT_EQ(index, value);
  }
}

TEST_F(ArrayListTest, ToString) {
  EXPECT_EQ(String("[1, 2, 3]"), list_.ToString());
}

}  // CommonTest
