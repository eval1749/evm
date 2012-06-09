#include "precomp.h"
// @(#)$Id$
//
// Evita IL - DataFlowAnalysisTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractIlTest.h"

#include "Il/Analysis/DataFlowAnalysis.h"

namespace IlTest {

class DataFlowAnalysisTest : public AbstractIlTest {
};

// (Values Int32) %v0 = prologue
// Int32 %r1 = select %v0 0
// ret %r1
TEST_F(DataFlowAnalysisTest, Basic) {
  auto& fun = NewFunction("Foo");
  fun.SetUp();
  auto& zone = module().zone();
  auto& bb1 = *fun.GetStartBB();
  auto& prologue = *bb1.GetFirstI();
  auto& r1 = module().NewRegister();
  bb1.AppendI(*new(zone) SelectI(*Ty_Int32, r1, *prologue.GetVd(), 0));
  bb1.AppendI(*new(zone) RetI(r1));

  ASSERT_TRUE(fun.ComputeLiveness());
  EXPECT_EQ(-1, fun.GetEntryBB()->Extend<DataFlowBB>()->GetIn().FindOne());
  EXPECT_EQ(-1, fun.GetEntryBB()->Extend<DataFlowBB>()->GetOut().FindOne());

  EXPECT_EQ(-1, fun.GetExitBB()->Extend<DataFlowBB>()->GetIn().FindOne());
  EXPECT_EQ(-1, fun.GetExitBB()->Extend<DataFlowBB>()->GetOut().FindOne());

  EXPECT_EQ(-1, fun.GetStartBB()->Extend<DataFlowBB>()->GetIn().FindOne());
  EXPECT_EQ(-1, fun.GetStartBB()->Extend<DataFlowBB>()->GetOut().FindOne());
}

}  // IlTest
