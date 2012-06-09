#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL - NewI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NewI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

NewI::NewI() : Base(Ty_Void, Void) {}
NewI::NewI(const Type& outy, const Register& out) : Base(outy, out) {}

} // Ir
} // Il
