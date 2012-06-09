#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Common.h"

using namespace Common;

class CharTest : public ::testing::Test {
};

TEST_F(CharTest, Basic) {
  auto const pCharA = Char::New('A');
  EXPECT_EQ('A', pCharA->GetCode());
  EXPECT_EQ(UnicodeCategory_UppercaseLetter, pCharA->GetUnicodeCategory());
}
