#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./JumpI.h"

#include "./PhiI.h"
#include "./PhiOperandBox.h"

#include "../CfgEdge.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

JumpI::JumpI() : Base(Ty_Void, Void) {}

JumpI::JumpI(const BBlock& bblock) :Base(Ty_Void, Void) {
  AppendOperand(bblock.label());
}

JumpI::JumpI(const BBlock* const bblock)
    : Base(Ty_Void, Void) {
  ASSERT(bblock != nullptr);
  AppendOperand(bblock->label());
} // JumpI

// [G]
Label* JumpI::GetLabel() const {
  return GetSx()->StaticCast<Label>(); 
} // GetLabel

BBlock* JumpI::GetTargetBB() const { 
  return GetLabel()->GetBB(); 
} // GetTargetBB

// [R]
void JumpI::Realize() {
  Instruction::Realize();
  GetBB()->AddEdge(GetTargetBB());
} // JumpI::Realize

// [U]
void JumpI::Unrealize() {
  auto const pTargetBB = GetTargetBB();
  GetBB()->RemoveOutEdge(pTargetBB);

  for (auto& phi_inst: pTargetBB->phi_instructions()) {
    if (auto pBox = phi_inst.FindOperandBox(GetBB()))
      phi_inst.RemoveOperand(pBox);
  }

  Instruction::Unrealize();
} // JumpI::Unrealize

} // Ir
} // Il
