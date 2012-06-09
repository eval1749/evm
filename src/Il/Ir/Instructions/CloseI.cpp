#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - CloseI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CloseI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
CloseI::CloseI() : Base(Ty_Void, Void) {}

CloseI::CloseI(FrameReg* const pRx) :
        Base(Ty_Void, Void) {
    this->AppendOperand(pRx);
} // CloseI

// [I]
bool CloseI::IsUseless() const {
    return GetSx()->StaticCast<FrameReg>()->GetDefI() == nullptr;
} // IsUseless

} // Ir
} // Il
