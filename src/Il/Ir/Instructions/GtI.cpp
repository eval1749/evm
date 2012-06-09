#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - GtI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./GtI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
GtI::GtI() : Base(Ty_Void, Void) {}

GtI::GtI(
    const BoolOutput& out,
    const Operand& s0,
    const Operand& s1)
    : Base(*Ty_Bool, out) {
  AppendOperand(s0);
  AppendOperand(s1);
}

GtI::GtI(
    const BoolOutput* const out,
    const Operand* const s0,
    const Operand* const s1)
    : Base(Ty_Bool, out) {
  AppendOperand(s0);
  AppendOperand(s1);
}

} // Ir
} // Il
