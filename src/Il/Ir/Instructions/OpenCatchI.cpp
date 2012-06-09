#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - OpenCatchI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./OpenCatchI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
OpenCatchI::OpenCatchI() : Base(Ty_Void, Void) {}

OpenCatchI::OpenCatchI(FrameReg* const pFd) :
    Base(&PointerType::Intern(*Ty_Void), pFd) {}

// [I]
// No Catch for this OpenCatch
bool OpenCatchI::IsUseless() const {
    if (this->IsNextClose()) {
        return true;
    } // if
    return GetOutput()->StaticCast<FrameReg>()->m_nCount == 0;
} // IsUseless

} // Ir
} // Il
