#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - UseI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./UseI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
UseI::UseI() : Base(Ty_Void, Void) {}

UseI::UseI(Output* pRx) :
    Base(Ty_Void, Void) {
    this->AppendOperand(pRx);
} // UseI

// [I]
bool UseI::IsUseless() const {
    if (SsaOutput* const pRx = GetSx()->DynamicCast<SsaOutput>())
    {
        if (NULL == pRx->GetDefI())
        {
            //CLOG(2, "<li>No open for ~S</li>", pRx);
            return true;
        }
        return pRx->GetDefI()->IsUseless();
    }

    return false;
} // UseI::IsUseless

} // Ir
} // Il
