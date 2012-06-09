#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - EntryI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./OpenFinallyI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
OpenFinallyI::OpenFinallyI() : Base(Ty_Void, Void) {}

OpenFinallyI::OpenFinallyI(
    FrameReg* const pFd,
    Function* const pFn,
    Values* const pVy) :
        Base(&PointerType::Intern(*Ty_Void), pFd) {
    this->AppendOperand(pFn);
    this->AppendOperand(pVy);
} // OpenFinallyI

// [I]
bool OpenFinallyI::IsUseless() const {
    return this->IsNextClose();
} // IsUseless

} // Ir
} // Il
