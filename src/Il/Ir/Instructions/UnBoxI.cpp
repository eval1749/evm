#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - UnBoxI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./UnBoxI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

UnBoxI::UnBoxI() : Base(Ty_Void, Void) {}

UnBoxI::UnBoxI(
    Type* const pty,
    Output* const pRd,
    Register* const pRx) :
        Base(pty, pRd) {
    this->AppendOperand(pRx);
} // UnBoxI

} // Ir
} // Il
