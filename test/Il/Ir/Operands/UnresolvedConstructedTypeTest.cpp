#include "precomp.h"
// @(#)$Id$
//
// Evita Il Ir UnresolvedConstructedType test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Il.h"

using namespace Il::Ir;

class UnresolvedConstructedTypeTest : public ::testing::Test {
};

TEST_F(UnresolvedConstructedTypeTest, CtorOne) {
  auto& foo = Name::Intern("foo");
  auto& foo_ref = *new NameRef(foo, SourceInfo("unittest", 1, 1));
  Type* const tyargv[] = { Ty_Int32 };
  Type::Array tyargs(tyargv, lengthof(tyargv));
  UnresolvedConstructedType unty(foo_ref, tyargs);
  EXPECT_EQ(&foo_ref, &unty.name_ref());
}

TEST_F(UnresolvedConstructedTypeTest, CtorZero) {
  auto& foo = Name::Intern("foo");
  NameRef foo_ref(foo, SourceInfo("unittest", 1, 1));
  Type::Array tyargs(0);
  UnresolvedConstructedType unty(foo_ref, tyargs);
  EXPECT_EQ(&foo_ref, &unty.name_ref());
}
