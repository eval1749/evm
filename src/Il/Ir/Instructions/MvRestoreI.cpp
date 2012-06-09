#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - MvRestoreI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MvRestoreI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

MvRestoreI::MvRestoreI() : Base(Ty_Void, Void) {}

MvRestoreI::MvRestoreI(Values* pVd, Register* pRx) :
        Base(Ty_Void, pVd) {
    this->AppendOperand(pRx);
} // MvRestoreI

MvRestoreI::MvRestoreI(Values* pVd, PseudoOutput* pRx) :
        Base(Ty_Void, pVd) {
    this->AppendOperand(pRx);
} // MvRestoreI


} // Ir
} // Il
