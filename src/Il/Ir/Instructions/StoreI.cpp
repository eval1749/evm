#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - StoreI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./StoreI.h"

#include "../Operands/PrimitiveType.h"
#include "../Operands/VoidOutput.h"

namespace Il {
namespace Ir {

StoreI::StoreI() : Base(Ty_Void, Void) {}

StoreI::StoreI(
    const Operand& r1,
    const Operand& s2)
    : Base(Ty_Void, Void) {
  AppendOperand(r1);
  AppendOperand(s2);
}

} // Ir
} // Il
