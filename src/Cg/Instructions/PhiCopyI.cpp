#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - PhiCopyI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./PhiCopyI.h"

namespace Il {
namespace Cg {

PhiCopyI::PhiCopyI() : Base(Ty_Void, Void) {}


PhiCopyI::PhiCopyI(
    const Type& pType,
    const CgOutput& pRd,
    const Operand& pSx)
    : Base(pType, pRd) {
  AppendOperand(pSx);
} // PhiCopyI

} // Cg
} // Il
