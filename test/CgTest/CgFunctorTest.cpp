#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CgTest.h"

#include <ostream>
#include "Cg/CgFunctor.h"

namespace CgTest {

using namespace Il::Cg;

class CgFunctorTest : public ::testing::Test {
};

class MockFunctor : public CgFunctor {
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
    foreach (Instruction::EnumOperand, operands, inst) {
      auto const operand = operands.Get();
      operand->Apply(this);
    }

    CgFunctor::Process(inst);
  }

  public: bool Seen(const char* const name) {
    return seen_set_.Contains(&Name::Intern(name));
  }

  #define DEF_SEEN(Type) \
    private: virtual void Process(Type* p) override { \
      seen_set_.Add(&Name::Intern(p->GetKind())); \
    }

  DEF_SEEN(AddI)

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    DEF_SEEN(mp_name ## I)

  #include "Cg/Instructions/CgInstructions.inc"

  // Operands
  DEF_SEEN(ClosedMarker)
  DEF_SEEN(FunLit)
  DEF_SEEN(MemSlot)
  DEF_SEEN(Physical)
  DEF_SEEN(StackSlot)
  DEF_SEEN(ThreadSlot)
  DEF_SEEN(VarHome)

  DISALLOW_COPY_AND_ASSIGN(MockFunctor);
};

TEST_F(CgFunctorTest, Instruction) {
  Module module;
  auto& bblock = module.NewBBlock();

  AddI add_inst;
  bblock.AppendI(add_inst);

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    mp_name ## I inst_ ## mp_name; \
    bblock.AppendI(inst_ ## mp_name);

  #include "Cg/Instructions/CgInstructions.inc"

  MockFunctor functor;
  bblock.Apply(&functor);

  // CgFunctor must handle IR instructions.
  EXPECT_TRUE(functor.Seen(AddI::Kind_()));

  #define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
    EXPECT_TRUE(functor.Seen(#mp_name));

  #include "Cg/Instructions/CgInstructions.inc"
}

TEST_F(CgFunctorTest, Operands) {
  Module module;
  auto& fun = module.NewFunction(
      nullptr, 
      Function::Flavor_Named,
      Name::Intern("Foo"));

  ExitI inst;

  ClosedMarker closed_marker(123);
  inst.AppendOperand(closed_marker);

  FunLit fun_lit(fun);
  inst.AppendOperand(fun_lit);

  MemSlot mem_slot(RegClass_Gpr, 4, 8);
  inst.AppendOperand(mem_slot);

  RegDesc reg_desc;
  Physical phy(reg_desc);
  inst.AppendOperand(phy);

  StackSlot stack_slot(RegClass_Gpr, 24, 4);
  inst.AppendOperand(stack_slot);

  ThreadSlot thread_slot(RegClass_Gpr, 8, 4);
  inst.AppendOperand(thread_slot);

  Variable var(Name::Intern("bar"));
  VarHome var_home(&var, RegClass_Gpr, 12, 4);
  inst.AppendOperand(var_home);

  MockFunctor functor;
  inst.Apply(&functor);

  EXPECT_TRUE(functor.Seen(ClosedMarker::Kind_()));
  EXPECT_TRUE(functor.Seen(FunLit::Kind_()));
  EXPECT_TRUE(functor.Seen(MemSlot::Kind_()));
  EXPECT_TRUE(functor.Seen(Physical::Kind_()));
  EXPECT_TRUE(functor.Seen(StackSlot::Kind_()));
  EXPECT_TRUE(functor.Seen(ThreadSlot::Kind_()));
  EXPECT_TRUE(functor.Seen(VarHome::Kind_()));
}

}  // CgFunctorTest
