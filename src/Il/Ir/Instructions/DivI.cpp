#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - DivI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./DivI.h"

#include "../Operands/PrimitiveType.h"
#include "../Operands/VoidOutput.h"

namespace Il {
namespace Ir {

DivI::DivI() : Base(Ty_Void, Void) {}

DivI::DivI(
    Type* const pty,
    Register* const pRd,
    Operand* const pSx,
    Operand* const pSy ) :
        Base(pty, pRd) {
    this->AppendOperand(pSx);
    this->AppendOperand(pSy);
} // DivI

} // Ir
} // Il
