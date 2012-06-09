#include "precomp.h"
// @(#)$Id$
//
// Evita Il Ir Class test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractIlTest.h"

namespace IlTest {

using namespace Il::Ir;

class ModuleTest : public AbstractIlTest {
};

TEST_F(ModuleTest, NewLiteralBoolean) {
  Module module;

  auto& false_lit = module.NewLiteral(false);
  EXPECT_EQ(BooleanLiteral(false), false_lit);
  EXPECT_EQ(&false_lit, &module.NewLiteral(false));

  auto& true_lit = module.NewLiteral(true);
  EXPECT_EQ(BooleanLiteral(true), true_lit);
  EXPECT_EQ(&true_lit, &module.NewLiteral(true));

  EXPECT_NE(false_lit, true_lit);
}

TEST_F(ModuleTest, NewLiteralInt16) {
  Module module;

  auto const negative = -1234;
  auto const positive = 1234;

  auto& positive_lit = module.NewLiteral(*Ty_Int16, positive);
  EXPECT_EQ(Int16Literal(positive), positive_lit);
  EXPECT_EQ(&positive_lit, &module.NewLiteral(*Ty_Int16, positive));

  auto& negative_lit = module.NewLiteral(*Ty_Int16, negative);
  EXPECT_EQ(Int16Literal(negative), negative_lit);
  EXPECT_EQ(&negative_lit, &module.NewLiteral(*Ty_Int16, negative));

  EXPECT_NE(positive_lit, negative_lit);
}

TEST_F(ModuleTest, NewLiteralInt32) {
  Module module;

  auto const negative = -1234;
  auto const positive = 1234;

  auto& positive_lit = module.NewLiteral(*Ty_Int32, positive);
  EXPECT_EQ(Int32Literal(positive), positive_lit);
  EXPECT_EQ(&positive_lit, &module.NewLiteral(*Ty_Int32, positive));

  auto& negative_lit = module.NewLiteral(*Ty_Int32, negative);
  EXPECT_EQ(Int32Literal(negative), negative_lit);
  EXPECT_EQ(&negative_lit, &module.NewLiteral(*Ty_Int32, negative));

  EXPECT_NE(&positive_lit, &negative_lit);
}

TEST_F(ModuleTest, NewLiteralInt64) {
  Module module;

  auto const negative = static_cast<int64>(-1234) << 32;
  auto const positive = static_cast<int64>(1234) << 32;

  auto& positive_lit = module.NewLiteral(*Ty_Int64, positive);
  EXPECT_EQ(Int64Literal(positive), positive_lit);
  EXPECT_EQ(&positive_lit, &module.NewLiteral(*Ty_Int64, positive));

  auto& negative_lit = module.NewLiteral(*Ty_Int64, negative);
  EXPECT_EQ(Int64Literal(negative), negative_lit);
  EXPECT_EQ(&negative_lit, &module.NewLiteral(*Ty_Int64, negative));

  EXPECT_NE(&positive_lit, &negative_lit);
}

TEST_F(ModuleTest, NewLiteralInt8) {
  Module module;

  auto const negative = -12;
  auto const positive = 12;

  auto& positive_lit = module.NewLiteral(*Ty_Int8, positive);
  EXPECT_EQ(Int8Literal(positive), positive_lit);
  EXPECT_EQ(&positive_lit, &module.NewLiteral(*Ty_Int8, positive));

  auto& negative_lit = module.NewLiteral(*Ty_Int8, negative);
  EXPECT_EQ(Int8Literal(negative), negative_lit);
  EXPECT_EQ(&negative_lit, &module.NewLiteral(*Ty_Int8, negative));

  EXPECT_NE(&positive_lit, &negative_lit);
}

} // IlTest
