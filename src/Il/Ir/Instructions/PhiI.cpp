#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./PhiI.h"

#include "./PhiOperandBox.h"
#include "./ValuesI.h"

#include "../CfgEdge.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
PhiI::PhiI() : Base(Ty_Void, Void) {}

PhiI::PhiI(const Type& outy, const Output& out)
    : Base(outy, out) {}

// [A]

void PhiI::AddOperand(
    const BBlock& bblock,
    const Operand& operand) {
  auto& box = *new PhiOperandBox(
      &const_cast<BBlock&>(bblock),
      &const_cast<Operand&>(operand));
  AppendOperandBox(&box);
}

void PhiI::AddOperand(const BBlock* const b, const Operand* const p) {
  ASSERT(b != nullptr);
  ASSERT(p != nullptr);
  AddOperand(*b, *p);
}

// [F]
PhiOperandBox* PhiI::FindOperandBox(const BBlock* const bblock) const {
  for (auto& phi_box: phi_operand_boxes()) {
    if (&phi_box.bblock() == bblock)
      return &phi_box;
  }
  return nullptr;
}

// [G]
PhiOperandBox* PhiI::GetOperandBox(const BBlock* const block) const {
  auto pBox = FindOperandBox(block);
  ASSERT(pBox != nullptr);
  return pBox;
}

Operand* PhiI::GetOperand(const BBlock* const block) const {
  return GetOperandBox(block)->GetOperand();
}

Register* PhiI::GetRx(const BBlock* const block) const {
  return GetOperand(block)->DynamicCast<Register>();
}

} // Ir
} // Il
