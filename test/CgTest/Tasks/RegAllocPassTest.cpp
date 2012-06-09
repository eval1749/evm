#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCgTest.h"

#include "Il.h"
#include "Cg/Tasks/RegAllocPass.h"

namespace CgTest {

using namespace Il::Cg;

class RegAllocPassTest : public AbstractCgTest {
};

TEST_F(RegAllocPassTest, Single) {
  auto& fun = module().NewFunction(
    nullptr,
    Function::Flavor_Named,
    Name::Intern("Foo"));

  fun.SetUp();

  auto& bblock = *fun.GetStartBB();

  auto& prologue = *bblock.GetFirstI()->StaticCast<PrologueI>();
  prologue.set_output_type(ValuesType::Intern(*Ty_Int32));

  auto& r1 = module().NewRegister();
  bblock.AppendI(*new(zone()) SelectI(*Ty_Int32, r1, *prologue.GetVd(), 0));
  bblock.AppendI(*new(zone()) RetI(r1));

  RegAllocPass pass(session(), fun.module());
  pass.Start();
}

} // CgTest
