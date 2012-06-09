#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../../AbstractIlTest.h"

namespace IlTest {

using namespace Il::Ir;

class ArrayTypeTest : public AbstractIlTest {
};

TEST_F(ArrayTypeTest, Enumerable) {
  auto& strarrty = ArrayType::Intern(*Ty_String);
  auto& strenum = Ty_Enumerable->Construct(*Ty_String);
  EXPECT_EQ(Subtype_Yes, strarrty.IsSubtypeOf(strenum));
}

} // IlTest
