#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - EltRefI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./EltRefI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
EltRefI::EltRefI() : Base(Ty_Void, Void) {}

EltRefI::EltRefI(
    const PointerType& outy,
    const Register& out,
    const Operand& array,
    const Operand& index)
    : Base(outy, out) {
  AppendOperand(array);
  AppendOperand(index);
}

} // Ir
} // Il
