#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LoadI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

LoadI::LoadI() : Base(Ty_Void, Void) {}

LoadI::LoadI(
    const Type& outy,
    const Output& out,
    const Operand& src)
    : Base(outy, out) {
  AppendOperand(src);
}

} // Ir
} // Il
