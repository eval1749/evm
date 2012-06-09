#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - SubI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./SubI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

SubI::SubI() : Base(Ty_Void, Void) {}

SubI::SubI(
    const Type& outy,
    const Register& out,
    const Operand& src0,
    const Operand& src1)
    : Base(outy, out) {
  AppendOperand(src0);
  AppendOperand(src1);
}

SubI::SubI(
    const Type* outy,
    const Register* out,
    const Operand* src0,
    const Operand* src1)
    : Base(outy, out) {
  AppendOperand(src0);
  AppendOperand(src1);
}

} // Ir
} // Il
