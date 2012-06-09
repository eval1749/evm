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

class PointerTypeTest : public AbstractIlTest {
};

TEST_F(PointerTypeTest, Basic) {
  auto& a = PointerType::Intern(*Ty_String);
  auto& b = PointerType::Intern(*Ty_String);
  EXPECT_EQ(a, b);
  EXPECT_EQ(*Ty_String, a.pointee_type());
}

TEST_F(PointerTypeTest, IsBound) {
  auto& a = PointerType::Intern(*Ty_String);
  EXPECT_TRUE(a.IsBound());

  auto& b = *new TypeVar();
  auto& c = PointerType::Intern(b);
  EXPECT_FALSE(b.IsBound());

  b.And(*Ty_String);
  b.Close();
  EXPECT_TRUE(c.IsBound());
}

TEST_F(PointerTypeTest, IsSubtypeOf) {
  auto& ptyString = PointerType::Intern(*Ty_String);
  auto& ptyObject  = PointerType::Intern(*Ty_Object);
  auto& ptyInt32 = PointerType::Intern(*Ty_Int32);
  EXPECT_EQ(Subtype_Yes, ptyString.IsSubtypeOf(ptyObject));
  EXPECT_EQ(Subtype_No, ptyObject.IsSubtypeOf(ptyString));
  EXPECT_EQ(Subtype_Unknown, ptyString.IsSubtypeOf(ptyInt32));
  EXPECT_EQ(Subtype_No, ptyString.IsSubtypeOf(*Ty_String));
}

} // IlTest
