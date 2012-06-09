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

class ConstructedClassTest : public AbstractIlTest {
};

TEST_F(ConstructedClassTest, Array) {
  auto& enum_string = Ty_Enumerable->Construct(*Ty_String);
  auto& arr_string = Ty_Array->Construct(*Ty_String);
  ArrayList_<String> strings;
  auto found = false;
  foreach (Class::EnumBaseSpec, base_specs, arr_string) {
    auto& base_spec = base_specs.Get();
    strings.Add(base_spec.ToString());
    if (base_spec == enum_string) {
      found = true;
    }
  }
  EXPECT_TRUE(found) << "base specs of " << arr_string << " is " << strings;
}

TEST_F(ConstructedClassTest, Identity) {
  auto& a = Ty_Array->Construct(*Ty_String);
  auto& b = Ty_Array->Construct(*Ty_String);
  EXPECT_EQ(a, b);
  EXPECT_EQ(&a, &b);
}

TEST_F(ConstructedClassTest, String) {
  auto& enum_string = Ty_Enumerable->Construct(*Ty_Char);
  ArrayList_<String> strings;
  auto found = false;
  foreach (Class::EnumBaseSpec, base_specs, *Ty_String) {
    auto& base_spec = base_specs.Get();
    strings.Add(base_spec.ToString());
    if (base_spec == enum_string) {
      found = true;
    }
  }
  EXPECT_TRUE(found) << "base specs of " << *Ty_String << " is " << strings;
}


} // IlTest
