#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - LtI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LtI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
LtI::LtI() : Base(Ty_Void, Void) {}

LtI::LtI(
    const BoolOutput& out,
    const Operand& s0,
    const Operand& s1)
    : Base(*Ty_Bool, out) {
  AppendOperand(s0);
  AppendOperand(s1);
}

LtI::LtI(
    const BoolOutput* const out,
    const Operand* const s0,
    const Operand* const s1)
    : Base(Ty_Bool, out) {
  AppendOperand(s0);
  AppendOperand(s1);
}

} // Ir
} // Il
