#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - EntryI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./VarDefI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

VarDefI::VarDefI() : Base(Ty_Void, Void) {}

VarDefI::VarDefI(
    const Class& cell_class,
    const Register& r0,
    const Variable& var,
    const Operand& r2)
    : Base(cell_class, r0) {
  if (!r0.GetVariable()) {
    const_cast<Register&>(r0).SetVariable(var);
  }
  const_cast<Variable&>(var).SetDefI(this);
  AppendOperand(var);
  AppendOperand(r2);
}

bool VarDefI::IsUseless() const {
 if (variable().m_cUpRefs) {
   return false;
 }

 //if (GetOutput()->Is<Pseudo>()) return true;
 return Instruction::IsUseless();
}

} // Ir
} // Il
