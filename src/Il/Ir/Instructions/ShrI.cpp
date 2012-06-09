#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - ShrI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ShrI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
ShrI::ShrI() : Base(Ty_Void, Void) {}

ShrI::ShrI(
    Type* const pty,
    Register* const pRd,
    Operand* const pSx,
    Operand* const pSy) :
        Base(pty, pRd) {
    this->AppendOperand(pSx);
    this->AppendOperand(pSy);
} // ShrI

} // Ir
} // Il
