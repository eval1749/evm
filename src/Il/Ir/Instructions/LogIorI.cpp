#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - LogIorI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LogIorI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

LogIorI::LogIorI() : Base(Ty_Void, Void) {}

LogIorI::LogIorI(
    Type* const pty,
    Register* const pRd,
    Operand* const pSx,
    Operand* const pSy ) :
        Base(pty, pRd) {
    this->AppendOperand(pSx);
    this->AppendOperand(pSy);
} // LogIorI

} // Ir
} // Il
