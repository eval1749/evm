#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL - NullI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NullI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

NullI::NullI() : Base(Ty_Void, Void) {}
NullI::NullI(const Type& outy, const Register& out) : Base(outy, out) {}

} // Ir
} // Il
