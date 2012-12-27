#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"
#include "Il.h"

using namespace Il::Ir;

class NamespaceTest : public ::testing::Test {
  protected: const Name& a_;
  protected: const Name& b_;
  protected: const Name& c_;
  private: Namespace& global_ns_;

  protected: NamespaceTest()
      : a_(Name::Intern("a")),
        b_(Name::Intern("b")),
        c_(Name::Intern("c")),
        global_ns_(Namespace::CreateGlobalNamespace()) {
  }

  public: ~NamespaceTest() {
    delete &global_ns_;
  }

  protected: Namespace& global_ns() const { return global_ns_; }

  protected: Namespace& NewNamespace(const Name& name) {
    return *new Namespace(global_ns_, name);
  }
};

TEST_F(NamespaceTest, Basic) {
  auto& ns = NewNamespace(a_);
  EXPECT_NE(0, ns.ComputeHashCode());
  EXPECT_EQ(a_, ns.name());
  EXPECT_EQ(global_ns(), ns.outer());
  EXPECT_TRUE(ns.Is<ClassOrNamespace>());
}

TEST_F(NamespaceTest, AddFind) {
  auto& ns = NewNamespace(a_);
  Int32Literal l1(1);
  ns.Add(b_, l1);
  EXPECT_EQ(&l1, ns.Find(b_));
  EXPECT_EQ(nullptr, ns.Find(a_));
}

TEST_F(NamespaceTest, EnumMember) {
  auto& ns = NewNamespace(a_);
  Int32Literal l1(1);
  Int32Literal l2(2);
  Int32Literal l3(3);
  ns.Add(a_, l1);
  ns.Add(b_, l2);
  ns.Add(c_, l3);

  const Name* namev[4];
  ::ZeroMemory(namev, sizeof(namev));
  for (auto const entry: ns.entries()) {
    auto const lit = entry.value()->StaticCast<Literal>();
    int val = lit->GetInt32();
    if (val >= 1 && val < lengthof(namev)) {
      namev[val] = entry.key();
    }
  }

  EXPECT_EQ(&a_, namev[1]);
  EXPECT_EQ(&b_, namev[2]);
  EXPECT_EQ(&c_, namev[3]);
}
