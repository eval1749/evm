#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - NeI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NeI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
NeI::NeI() : Base(Ty_Void, Void) {}

NeI::NeI(
    const BoolOutput& out,
    const Operand& s0,
    const Operand& s1)
    : Base(*Ty_Bool, out) {
  AppendOperand(s0);
  AppendOperand(s1);
}

NeI::NeI(
    const BoolOutput* const out,
    const Operand* const s0,
    const Operand* const s1)
    : Base(Ty_Bool, out) {
  AppendOperand(s0);
  AppendOperand(s1);
}

} // Ir
} // Il
