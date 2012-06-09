#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - MvSaveI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MvSaveI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

MvSaveI::MvSaveI() : Base(Ty_Void, Void) {}

MvSaveI::MvSaveI(Register* pRd, Values* pVx) :
    Base(Ty_Object, pRd)
{
    AppendOperand(pVx);
} // MvSaveI

MvSaveI::MvSaveI(PseudoOutput* pRd, Values* pVx) :
    Base(Ty_Object, pRd)
{
    AppendOperand(pVx);
} // MvSaveI

} // Ir
} // Il
