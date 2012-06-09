#include "precomp.h"
// @(#)$Id$
//
// Util Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractCommonTest.h"

namespace CommonTest {

class FormatterTest : public AbstractCommonTest {
};

TEST_F(FormatterTest, Int32) {
  EXPECT_EQ(String("foo=1234"), String::Format("foo=%d", 1234));
  EXPECT_EQ(String("foo=-5678"), String::Format("foo=%d", -5678));
  EXPECT_EQ(String("foo=ab03"), String::Format("foo=%x", 0xAB03));
  EXPECT_EQ(String("foo=AB03"), String::Format("foo=%X", 0xAB03));
  EXPECT_EQ(String("foo=-AB03"), String::Format("foo=%X", -0xAB03));
}


TEST_F(FormatterTest, Int64) {
  EXPECT_EQ(String("foo=1234"), String::Format("foo=%d", 1234ll));
  EXPECT_EQ(String("foo=-5678"), String::Format("foo=%d", -5678ll));
}

TEST_F(FormatterTest, Object) {
  StringBuilder obj;
  char16 buf[20];
  ::wsprintfW(buf, L"obj=%p", &obj);
  EXPECT_EQ(String(buf), String::Format("obj=%p", &obj));
}

TEST_F(FormatterTest, Pointer) {
  char16 buf[20];
  ::wsprintfW(buf, L"ptr=%p", &buf);
  EXPECT_EQ(String(buf), String::Format("ptr=%p", &buf));
}

TEST_F(FormatterTest, String) {
  EXPECT_EQ(String("str=foo"), String::Format("str=%s", L"foo"));
}

TEST_F(FormatterTest, UInt32) {
  EXPECT_EQ(String("foo=1234"), String::Format("foo=%d", 1234u));
}

} // CommonTest
