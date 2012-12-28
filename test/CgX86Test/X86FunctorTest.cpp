#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CgX86Test.h"

#include "Targets/x86/Cg/X86Functor.h"

namespace CgX86Test {

using namespace Il::Cg;

class X86FunctorTest : public ::testing::Test {
};

class MockFunctor : public X86Functor {
  private: HashSet_<const Name*> seen_set_;
  public: MockFunctor() {}
  public: virtual ~MockFunctor() {}

  private: virtual void Process(BBlock* bblock) override {
    foreach (BBlock::EnumI, insts, bblock) {
      auto const inst = insts.Get();
      inst->Apply(this);
    }
  }

  private: virtual void Process(Instruction* inst) override {
    for (auto& operand: inst->operands())
      operand.Apply(this);

    X86Functor::Process(inst);
  }

  public: bool Seen(const char* const name) {
    return seen_set_.Contains(&Name::Intern(name));
  }

  // Probes
  private: virtual void Process(TttnLit*) override {
    seen_set_.Add(&Name::Intern("TttnLit"));
  }

  private: virtual void Process(x86ZeroI*) override {
    seen_set_.Add(&Name::Intern("x86ZeroI"));
  }

  DISALLOW_COPY_AND_ASSIGN(MockFunctor);
};

TEST_F(X86FunctorTest, Instruction) {
  Module module;
  auto& bblock = module.NewBBlock();
  x86ZeroI inst;
  bblock.AppendI(inst);
  MockFunctor functor;
  bblock.Apply(&functor);
  EXPECT_TRUE(functor.Seen("x86ZeroI"));
}

TEST_F(X86FunctorTest, TttnLit) {
  TttnLit tttn_lit(tttn_NE);
  RetI ret_inst(tttn_lit);
  MockFunctor functor;
  ret_inst.Apply(&functor);
  EXPECT_TRUE(functor.Seen("TttnLit"));
}

}  // X86FunctorTest
