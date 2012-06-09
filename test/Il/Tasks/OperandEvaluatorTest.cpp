#include "precomp.h"
// @(#)$Id$
//
// OperandEvaluatorTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractIlTaskTest.h"

#include "Il/Tasks/OperandEvaluator.h"

namespace IlTest {

using namespace Il::Tasks;

class OperandEvaluatorTest : public AbstractIlTaskTest {
  protected: BBlock& bblock_;
  protected: OperandEvaluator eval_;
  protected: Function& fn_;

  protected: OperandEvaluatorTest()
    : bblock_(NewBBlock(1)),
      eval_(session(), module()),
      fn_(NewFunction("foo")) {
    fn_.AppendBBlock(&bblock_);
  }
};

TEST_F(OperandEvaluatorTest, Eq_True) {
  BoolOutput b1;
  BoolOutput b2;

  EqI eq_inst(b1, b2, *True);
  bblock_.AppendI(eq_inst);

  auto& result = eval_.Eval(b1);
  EXPECT_EQ(b2, result);
}

TEST_F(OperandEvaluatorTest, Eq_True2) {
  BoolOutput b1;
  BoolOutput b2;

  EqI eq_inst(b1, *True, b2);
  bblock_.AppendI(eq_inst);

  auto& result = eval_.Eval(b1);
  EXPECT_EQ(b2, result);
}

TEST_F(OperandEvaluatorTest, Eq_If) {
  auto& r1 = module().NewRegister();
  BoolOutput b2;
  BoolOutput b3;

  IfI if_inst(*Ty_Boolean, r1, b2, LitTrue(), LitFalse());
  EqI eq_inst(b3, r1, LitTrue());

  bblock_.AppendI(if_inst);
  bblock_.AppendI(eq_inst);

  auto& result = eval_.Eval(b3);
  EXPECT_EQ(b2, result);
}

TEST_F(OperandEvaluatorTest, If_Bool) {
  BoolOutput b1;
  BoolOutput b2;

  IfI if_inst(*Ty_Bool, b1, b2, *True, *False);
  bblock_.AppendI(if_inst);

  auto& result = eval_.Eval(b1);
  EXPECT_EQ(b2, result);
}


TEST_F(OperandEvaluatorTest, If_Not_Not) {
  BoolOutput b1;
  BoolOutput b2;
  BoolOutput b3;

  IfI if_inst1(*Ty_Bool, b1, b2, *False, *True);
  IfI if_inst2(*Ty_Bool, b3, b1, *False, *True);

  bblock_.AppendI(if_inst1);
  bblock_.AppendI(if_inst2);

  auto& result = eval_.Eval(b3);
  EXPECT_EQ(b2, result);
}

TEST_F(OperandEvaluatorTest, If_Same) {
  auto& r1 = module().NewRegister();
  BoolOutput b2;
  auto& r3 = module().NewRegister();

  IfI if_inst(*Ty_Boolean, r1, b2, r3, r3);
  bblock_.AppendI(if_inst);

  auto& result = eval_.Eval(r1);
  EXPECT_EQ(r3, result);
}

} // IlTest
