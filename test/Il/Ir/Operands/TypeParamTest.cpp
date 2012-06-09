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

class TypeParamTest : public AbstractIlTest {
};

TEST_F(TypeParamTest, IsSubtypeOf) {
  auto& typaram = *new TypeParam(Name::Intern("T"));
  auto& tyvar = *new TypeVar();
  EXPECT_EQ(Subtype_Unknown, typaram.IsSubtypeOf(tyvar));
}

} // IlTest
