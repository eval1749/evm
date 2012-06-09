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

class GenericClassTest : public AbstractIlTest {
};

TEST_F(GenericClassTest, IsBound) {
  EXPECT_FALSE(Ty_Enumerable->IsBound());
}

} // IlTest
