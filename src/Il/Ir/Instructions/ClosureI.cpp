#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - ClosureI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ClosureI.h"

#include "./FunctionOperandBox.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

ClosureI::ClosureI() : Base(Ty_Void, Void) {}

ClosureI::ClosureI(
    FunctionType* const pFunty,
    Register*           const pRd,
    Function*           const pFun,
    Values*             const pVy ) :
        Base(pFunty, pRd) {
    this->AppendOperandBox(new FunctionOperandBox(pFun));
    this->AppendOperand(pVy);
} // ClosureI

} // Ir
} // Il
