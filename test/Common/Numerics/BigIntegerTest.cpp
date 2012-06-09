#include "precomp.h"
// @(#)$Id$
//
// Util Test for BigInteger
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCommonTest.h"

#include "Common/Numerics/BigInteger.h"

namespace CommonTest {

using namespace Common::Numerics;

class BigIntegerTest : public AbstractCommonTest {
  public: static String Add(int a, int b) {
    return (BigInteger(a) + BigInteger(b)).ToString();
  }
};

TEST_F(BigIntegerTest, Add) {
  EXPECT_STREQ(L"0", Add(0, 0).value());
  EXPECT_STREQ(L"1", Add(0, 1).value());
  EXPECT_STREQ(L"1", Add(1, 0).value());
  EXPECT_STREQ(L"2", Add(1, 1).value());

  EXPECT_STREQ(L"-1", Add(0, -1).value());
  EXPECT_STREQ(L"-1", Add(-1, 0).value());
  EXPECT_STREQ(L"-2", Add(-1, -1).value());
  EXPECT_STREQ(L"0", Add(-1, 1).value());
}

TEST_F(BigIntegerTest, Pow) {
  EXPECT_EQ(BigInteger(0), BigInteger(0).Pow(1));
  EXPECT_EQ(BigInteger(1), BigInteger(2).Pow(0));
  EXPECT_EQ(BigInteger(1024), BigInteger(2).Pow(10));
}

} // CommonTest
