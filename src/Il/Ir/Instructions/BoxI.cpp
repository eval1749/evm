#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - BoxI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./BoxI.h"

#include "../Operands/PrimitiveType.h"
#include "../Operands/VoidOutput.h"

namespace Il {
namespace Ir {

BoxI::BoxI() : Base(Ty_Void, Void) {}

BoxI::BoxI(
    const Type& outy,
    const Output& out,
    const Operand& s0)
    : Base(outy, out) {
  AppendOperand(s0);
}

BoxI::BoxI(
    const Type* const outy,
    const Output* const out,
    const Operand* const s0)
    : Base(outy, out) {
  AppendOperand(s0);
}

} // Ir
} // Il
