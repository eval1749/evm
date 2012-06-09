#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - LogAndI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LogAndI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

LogAndI::LogAndI() : Base(Ty_Void, Void) {}

LogAndI::LogAndI(
    Type* const pty,
    Register* const pRd,
    Operand* const pSx,
    Operand* const pSy ) :
        Base(pty, pRd) {
    this->AppendOperand(pSx);
    this->AppendOperand(pSy);
} // LogAndI

} // Ir
} // Il
