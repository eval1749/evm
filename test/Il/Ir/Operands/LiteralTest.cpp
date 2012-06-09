#include "precomp.h"
// @(#)$Id$
//
// Evita Il Ir Class test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../../AbstractIlTest.h"

namespace IlTest {

using namespace Il::Ir;

class LiteralTest : public AbstractIlTest {
};

TEST_F(LiteralTest, Boolean) {
  BooleanLiteral false_lit(false);
  EXPECT_TRUE(false_lit.Is<BooleanLiteral>());
  EXPECT_FALSE(false_lit.value());
  EXPECT_EQ(String("false"), false_lit.ToString());

  BooleanLiteral true_lit(true);
  EXPECT_TRUE(true_lit.Is<BooleanLiteral>());
  EXPECT_TRUE(true_lit.value());
  EXPECT_EQ(String("true"), true_lit.ToString());
}

TEST_F(LiteralTest, Int16) {
  Int16Literal positive_lit(1234);
  EXPECT_TRUE(positive_lit.Is<Int16Literal>());
  EXPECT_EQ(1234, positive_lit.value());
  EXPECT_EQ(String("int16(1234)"), positive_lit.ToString());

  Int16Literal zero_lit(0);
  EXPECT_TRUE(zero_lit.Is<Int16Literal>());
  EXPECT_EQ(0, zero_lit.value());
  EXPECT_EQ(String("int16(0)"), zero_lit.ToString());

  Int16Literal negative_lit(-1234);
  EXPECT_TRUE(negative_lit.Is<Int16Literal>());
  EXPECT_EQ(-1234, negative_lit.value());
  EXPECT_EQ(String("int16(-1234)"), negative_lit.ToString());
}

TEST_F(LiteralTest, Int32) {
  Int32Literal positive_lit(1234);
  EXPECT_TRUE(positive_lit.Is<Int32Literal>());
  EXPECT_EQ(1234, positive_lit.value());
  EXPECT_EQ(String("1234"), positive_lit.ToString());

  Int32Literal zero_lit(0);
  EXPECT_TRUE(zero_lit.Is<Int32Literal>());
  EXPECT_EQ(0, zero_lit.value());
  EXPECT_EQ(String("0"), zero_lit.ToString());

  Int32Literal negative_lit(-1234);
  EXPECT_TRUE(negative_lit.Is<Int32Literal>());
  EXPECT_EQ(-1234, negative_lit.value());
  EXPECT_EQ(String("-1234"), negative_lit.ToString());
}

} // IlTest
