#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Il.h"

namespace IlTest {

using namespace Il::Ir;

class NameTest : public ::testing::Test {
};

TEST_F(NameTest, Basic) {
  auto& a = Name::Intern("a");
  EXPECT_EQ(Name::Intern("a"), a);
}

TEST_F(NameTest, ComputeHashCode) {
  auto& a = Name::Intern("a");
  auto& a2 = Name::Intern("a");
  auto& b = Name::Intern("b");
  auto const hash_code = a.ComputeHashCode();
  EXPECT_EQ(a, a2);
  EXPECT_EQ(hash_code, a2.ComputeHashCode());
  EXPECT_NE(b.ComputeHashCode(), hash_code);
}

TEST_F(NameTest, Equals) {
  auto& a = Name::Intern("a");
  auto& a2 = Name::Intern("a");
  EXPECT_TRUE(a == a2);
}

TEST_F(NameTest, ToString) {
  auto& a = Name::Intern("a");
  EXPECT_EQ(String("a"), a.ToString());
}

}  // IlTest
