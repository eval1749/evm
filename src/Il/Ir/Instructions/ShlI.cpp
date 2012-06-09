#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - ShlI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ShlI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
ShlI::ShlI() : Base(Ty_Void, Void) {}

ShlI::ShlI(
    const Type& outy,
    const Register& out,
    const Operand& src1,
    const Operand& src2)
    : Base(outy, out) {
  AppendOperand(src1);
  AppendOperand(src2);
}

ShlI::ShlI(
    const Type* const outy,
    const Register* const out,
    const Operand* const src1,
    const Operand* const src2)
    : Base(outy, out) {
  AppendOperand(src1);
  AppendOperand(src2);
}

} // Ir
} // Il
