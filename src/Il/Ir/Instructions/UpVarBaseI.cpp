#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./UpVarBaseI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

UpVarBaseI::UpVarBaseI() : Base(Ty_Void, Void) {}

UpVarBaseI::UpVarBaseI(
    Register* const pRd,
    Function* const pOwner) :
        Base(Ty_Object, pRd) {
    ASSERT(nullptr != pOwner);
    ASSERT(nullptr != pRd);
    this->AppendOperand(pOwner);
} // UpVarBaseI

} // Ir
} // Il
