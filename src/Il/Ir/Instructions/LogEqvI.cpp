#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - LogEqvI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LogEqvI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

LogEqvI::LogEqvI() : Base(Ty_Void, Void) {}

LogEqvI::LogEqvI(
    Type* const pty,
    Register* const pRd,
    Operand* const pSx,
    Operand* const pSy ) :
        Base(pty, pRd) {
    this->AppendOperand(pSx);
    this->AppendOperand(pSy);
} // LogEqvI

} // Ir
} // Il
