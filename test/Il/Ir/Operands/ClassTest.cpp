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

// class A {}
// interface B {}
// interface C {}
// interface D : B {}
// class E : A, C, D {}
class ClassTest : public AbstractIlTest {
  protected: Class& class_A;
  protected: Class& iface_B;
  protected: Class& iface_C;
  protected: Class& iface_D;
  protected: Class& class_E;

  protected: ClassTest()
    : class_A(NewClass("A", *Ty_Object)),
      iface_B(NewInterface("B")),
      iface_C(NewInterface("C")),
      iface_D(NewInterface("D", iface_B)),
      class_E(NewClass("E", class_A, iface_C, iface_D)) {}
};

TEST_F(ClassTest, EnumBaseSpec) {
  Type::Set tyset;
  foreach (Class::EnumBaseSpec, classes, iface_D) {
    tyset.Add(&classes.Get());
  }
  EXPECT_EQ(1, tyset.Count());
  EXPECT_TRUE(tyset.Contains(&iface_B));
}

TEST_F(ClassTest, EnumBaseSpec2) {
  Type::Set tyset;
  foreach (Class::EnumBaseSpec, classes, class_E) {
    tyset.Add(&classes.Get());
  }
  EXPECT_EQ(3, tyset.Count()); // A, C, and D
  EXPECT_TRUE(tyset.Contains(&class_A));
  EXPECT_TRUE(tyset.Contains(&iface_C));
  EXPECT_TRUE(tyset.Contains(&iface_D));
}

TEST_F(ClassTest, EnumClassTree) {
  Type::Set tyset;
  foreach (Class::EnumClassTree, classes, class_E) {
    tyset.Add(&classes.Get());
  }
  EXPECT_EQ(6, tyset.Count()); // A, B, C, D, E and Object.
  EXPECT_TRUE(tyset.Contains(&class_A));
  EXPECT_TRUE(tyset.Contains(&iface_B));
  EXPECT_TRUE(tyset.Contains(&iface_C));
  EXPECT_TRUE(tyset.Contains(&iface_D));
  EXPECT_TRUE(tyset.Contains(&class_E));
  EXPECT_TRUE(tyset.Contains(Ty_Object));
}

TEST_F(ClassTest, IsSubtypeOf) {
  EXPECT_EQ(Subtype_Yes, Ty_String->IsSubtypeOf(*Ty_Object));
  EXPECT_EQ(Subtype_No, Ty_Object->IsSubtypeOf(*Ty_String));
  EXPECT_EQ(Subtype_Unknown, Ty_String->IsSubtypeOf(*Ty_Int32));
  EXPECT_EQ(Subtype_Unknown, Ty_Int32->IsSubtypeOf(*Ty_String));
}

} // IlTest
