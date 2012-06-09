#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - MulI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MulI.h"

#include "../Operands/PrimitiveType.h"
#include "../Operands/VoidOutput.h"

namespace Il {
namespace Ir {

MulI::MulI() : Base(Ty_Void, Void) {}

MulI::MulI(
    const Type& outy,
    const Register& out,
    const Operand& op0,
    const Operand& op1)
    : Base(outy, out) {
  AppendOperand(op0);
  AppendOperand(op1);
}

MulI::MulI(
    const Type* const pty,
    const Register* const pRd,
    const Operand* const pSx,
    const Operand* const pSy )
    : Base(pty, pRd) {
  AppendOperand(pSx);
  AppendOperand(pSy);
}

} // Ir
} // Il
