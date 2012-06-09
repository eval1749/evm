#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - NonLocalI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NonLocalI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
NonLocalI::NonLocalI() : Base(Ty_Void, Void) {}

NonLocalI::NonLocalI(const Type& outy, const Values& out)
    : Base(outy, out) {}

} // Ir
} // Il
