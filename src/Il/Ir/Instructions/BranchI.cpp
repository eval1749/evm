#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - BranchI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./BranchI.h"

#include "../CfgEdge.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

BranchI::BranchI() : Base(Ty_Void, Void) {}

BranchI::BranchI(
    const BoolOutput& bx,
    const BBlock& true_bb,
    const BBlock& false_bb)
    : Base(Ty_Void, Void) {
  AppendOperand(bx);
  AppendOperand(true_bb.label());
  AppendOperand(false_bb.label());
}

// [G]
BBlock* BranchI::GetFalseBB() const {
  return GetSz()->StaticCast<Label>()->GetBB();
}

BBlock* BranchI::GetTrueBB() const {
  return GetSy()->StaticCast<Label>()->GetBB();
}

void BranchI::Realize() {
  Instruction::Realize();
  GetBB()->AddEdge(GetTrueBB());
  GetBB()->AddEdge(GetFalseBB());
} // BranchI::Realize

void BranchI::Unrealize() {
  GetBB()->RemoveOutEdge(GetTrueBB());
  GetBB()->RemoveOutEdge(GetFalseBB());
  Instruction::Unrealize();
} // BranchI::Unrealize

} // Ir
} // Il
