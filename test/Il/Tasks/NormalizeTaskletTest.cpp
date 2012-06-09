#include "precomp.h"
// @(#)$Id$
//
// NormalizeTaskletTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractIlTaskTest.h"

#include "Il/Tasks/NormalizeTasklet.h"

namespace IlTest {

using namespace Il::Tasks;

class NormalizeTaskletTest : public AbstractIlTaskTest {
  protected: Function& fn_;
  protected: NormalizeTasklet normalizer_;
  protected: int num_sources_;

  protected: NormalizeTaskletTest()
    : fn_(NewFunction("foo")),
      normalizer_(session(), module()),
      num_sources_(0) {
    fn_.SetUp();
  }

  protected: void EmitI(const Instruction& inst) {
    fn_.GetStartBB()->AppendI(inst);
  }

  protected: const Operand& NewInt32(int32 i32) {
    return module().NewLiteral(*Ty_Int32, i32);
  }

  protected: const Register& NewSource(const Type& type) {
    auto& out = module().NewRegister();
    auto& prologue_inst = *fn_.GetPrologueI();
    auto& values = *prologue_inst.output().StaticCast<Values>();
    EmitI(*new(module().zone())
        SelectI(type, out, values, num_sources_));
    ++num_sources_;
    ValuesTypeBuilder builder(
        *prologue_inst.output_type().StaticCast<ValuesType>());
    builder.Append(type);
    prologue_inst.set_output_type(builder.GetValuesType());
    return out;
  }
};

TEST_F(NormalizeTaskletTest, Mul2) {
  auto& r0 = NewSource(*Ty_Int32);
  auto& r1 = module().NewRegister();
  MulI mul_inst(*Ty_Int32, r1, r0, NewInt32(2));
  EmitI(mul_inst);

  RetI ret_inst(r1);
  EmitI(ret_inst);

  normalizer_.Add(mul_inst);
  normalizer_.Start();

  auto& add_inst = *ret_inst.GetRx()->GetDefI();
  EXPECT_EQ(Op_Add, add_inst.opcode());
  EXPECT_EQ(r0, add_inst.op0());
  EXPECT_EQ(r0, add_inst.op1());
}

TEST_F(NormalizeTaskletTest, Mul4) {
  auto& r0 = NewSource(*Ty_Int32);
  auto& r1 = module().NewRegister();
  MulI mul_inst(*Ty_Int32, r1, r0, NewInt32(4));
  EmitI(mul_inst);

  RetI ret_inst(r1);
  EmitI(ret_inst);

  normalizer_.Add(mul_inst);
  normalizer_.Start();

  auto& shl_inst = *ret_inst.GetRx()->GetDefI();
  EXPECT_EQ(Op_Shl, shl_inst.opcode());
  EXPECT_EQ(r0, shl_inst.op0());
  EXPECT_EQ(NewInt32(2), shl_inst.op1());
}

TEST_F(NormalizeTaskletTest, MulMinus2) {
  auto& r0 = NewSource(*Ty_Int32);
  auto& r1 = module().NewRegister();
  MulI mul_inst(*Ty_Int32, r1, r0, NewInt32(-2));
  EmitI(mul_inst);

  RetI ret_inst(r1);
  EmitI(ret_inst);

  normalizer_.Add(mul_inst);
  normalizer_.Start();

  auto& sub_inst = *ret_inst.GetRx()->GetDefI();
  EXPECT_EQ(Op_Sub, sub_inst.opcode());
  EXPECT_EQ(NewInt32(0), sub_inst.op0());

  auto& add_inst = *sub_inst.GetRy()->GetDefI();
  EXPECT_EQ(Op_Add, add_inst.opcode());
  EXPECT_EQ(r0, add_inst.op0());
  EXPECT_EQ(r0, add_inst.op1());
}

TEST_F(NormalizeTaskletTest, MulMinus4) {
  auto& r0 = NewSource(*Ty_Int32);
  auto& r1 = module().NewRegister();
  MulI mul_inst(*Ty_Int32, r1, r0, NewInt32(-4));
  EmitI(mul_inst);

  RetI ret_inst(r1);
  EmitI(ret_inst);

  normalizer_.Add(mul_inst);
  normalizer_.Start();

  auto& sub_inst = *ret_inst.GetRx()->GetDefI();
  EXPECT_EQ(Op_Sub, sub_inst.opcode());
  EXPECT_EQ(NewInt32(0), sub_inst.op0());

  auto& shl_inst = *sub_inst.GetRy()->GetDefI();
  EXPECT_EQ(Op_Shl, shl_inst.opcode());
  EXPECT_EQ(r0, shl_inst.op0());
  EXPECT_EQ(NewInt32(2), shl_inst.op1());
}

} // IlTest
