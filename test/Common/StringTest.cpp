#include "precomp.h"
// @(#)$Id$
//
// Evita String Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common.h"

using namespace Common;

class StringTest : public ::testing::Test {
};

TEST_F(StringTest, CompareTo) {
  EXPECT_GT(String("foo").CompareTo("bar"), 0);
  EXPECT_LT(String("bar").CompareTo("foo"), 0);
  EXPECT_GT(String("x").CompareTo("abc"), 0);
  EXPECT_EQ(0, String("x").CompareTo("x"));
}

TEST_F(StringTest, Ctor) {
  String foo("foo");
  EXPECT_STREQ(L"foo", foo.value());

  String bar(L"bar");
  EXPECT_STREQ(L"bar", bar.value());
}

TEST_F(StringTest, Concat) {
  String foo("foo");
  auto foobar = foo.Concat(L"bar");
  EXPECT_STREQ(L"foobar", foobar.value());
}

TEST_F(StringTest, Copy) {
 String foo1("foo");
 String foo2(foo1);
 EXPECT_EQ(foo1, foo2);
 EXPECT_EQ(foo1.value(), foo2.value());
}

TEST_F(StringTest, IndexOf) {
  String foobar("foobar");
  EXPECT_EQ(1, foobar.IndexOf('o'));
  EXPECT_EQ(3, foobar.IndexOf('b'));
  EXPECT_EQ(-1, foobar.IndexOf('x'));
}


TEST_F(StringTest, LastIndexOf) {
  String foobar("foobar");
  EXPECT_EQ(2, foobar.LastIndexOf('o'));
  EXPECT_EQ(3, foobar.LastIndexOf('b'));
  EXPECT_EQ(-1, foobar.LastIndexOf('x'));
}

TEST_F(StringTest, Operator) {
 String foo1("foo");
 String foo2("foo");
 String bar1("bar");
 EXPECT_TRUE(foo1 == foo2);
 EXPECT_FALSE(foo1 != foo2);
 EXPECT_TRUE(foo1 != bar1);
}

TEST_F(StringTest, Substring) {
  String foobar("foobar");
  EXPECT_EQ(String("foo"), foobar.Substring(0, 3));
  EXPECT_EQ(String("bar"), foobar.Substring(3));
  EXPECT_EQ(String(), foobar.Substring(6));
}
