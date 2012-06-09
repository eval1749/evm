#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - IfI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./IfI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
IfI::IfI() : Base(Ty_Void, Void) {}

IfI::IfI(
    const Type& type,
    const Output& r1,
    const BoolOutput& b2,
    const Operand& s3,
    const Operand& s4)
    : Base(&type, &r1) {
  AppendOperand(b2);
  AppendOperand(s3);
  AppendOperand(s4);
} // IfI

// [I]
bool IfI::IsNot(const Instruction& inst) {
  if (auto const if_inst = inst.DynamicCast<IfI>()) {
    return if_inst->GetSy() == False && if_inst->GetSz() == True;
  }
  return false;
}

} // Ir
} // Il
