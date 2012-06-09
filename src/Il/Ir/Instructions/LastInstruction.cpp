#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LastInstruction.h"

namespace Il {
namespace Ir {

LastInstruction::LastInstruction(
    Op const eOp,
    const Type* const pType,
    const Output* pOutput) :
        Base(eOp, pType, pOutput) {}

} // Ir
} // Il
