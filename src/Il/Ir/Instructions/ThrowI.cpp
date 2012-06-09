#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - ThrowI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ThrowI.h"

#include "./ValuesI.h"

#include "../Operands/PrimitiveType.h"
#include "../Operands/VoidOutput.h"

namespace Il {
namespace Ir {

// ctor
ThrowI::ThrowI() : Base(Ty_Void, Void) {}

ThrowI::ThrowI(Operand* const pSx) :
        Base(Ty_Void, Void) {
    ASSERT(nullptr != pSx);
    this->AppendOperand(pSx);
} // ThrowI

} // Ir
} // Il
