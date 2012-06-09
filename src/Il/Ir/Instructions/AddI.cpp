#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - AddI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AddI.h"

#include "../Operands/PrimitiveType.h"
#include "../Operands/VoidOutput.h"

namespace Il {
namespace Ir {

AddI::AddI() : Base(Ty_Void, Void) {}

AddI::AddI(
    const Type& outy,
    const Register& out,
    const Operand& op0,
    const Operand& op1)
    : Base(outy, out) {
  AppendOperand(op0);
  AppendOperand(op1);
}

AddI::AddI(
    Type* const pty,
    Register* const pRd,
    Operand* const pSx,
    Operand* const pSy )
    : Base(pty, pRd) {
  AppendOperand(pSx);
  AppendOperand(pSy);
}

} // Ir
} // Il
