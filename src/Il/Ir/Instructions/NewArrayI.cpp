#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL - NewArrayI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NewArrayI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

NewArrayI::NewArrayI() : Base(Ty_Void, Void) {}

NewArrayI::NewArrayI(
    const ArrayType& outy,
    const Register& out,
    const Values& values) : Base(outy, out) {
  AppendOperand(values);
}

} // Ir
} // Il
