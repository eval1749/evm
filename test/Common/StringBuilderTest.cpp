#include "precomp.h"
// @(#)$Id$
//
// Evita String Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common.h"
//#include "Common/Io.h"

using namespace Common;

class StringBuilderTest : public ::testing::Test {
};

TEST_F(StringBuilderTest, Basic) {
  StringBuilder buf;
  buf.Append('a');
  buf.Append("bcde");
  buf.AppendFormat("%s", 69);
  EXPECT_STREQ(L"abcde69", buf.GetString());
  EXPECT_EQ(7, buf.GetLength());

  buf.Clear();
  EXPECT_STREQ(L"", buf.GetString());
  EXPECT_EQ(0, buf.GetLength());
}
