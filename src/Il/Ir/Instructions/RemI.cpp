#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - RemI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./RemI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

RemI::RemI() : Base(Ty_Void, Void) {}

RemI::RemI(
    Type* const pty,
    Register* const pRd,
    Operand* const pSx,
    Operand* const pSy ) :
        Base(pty, pRd) {
    this->AppendOperand(pSx);
    this->AppendOperand(pSy);
} // RemI

} // Ir
} // Il
