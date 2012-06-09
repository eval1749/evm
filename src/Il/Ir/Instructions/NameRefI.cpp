#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL - NameRefI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NameRefI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

//ctor
NameRefI::NameRefI() : Base(Ty_Void, Void) {}

NameRefI::NameRefI(
    const Type& outy,
    const Register& out,
    const Operand& base,
    const NameRef& name_ref)
    : Base(outy, out) {
  AppendOperand(base);
  AppendOperand(name_ref);
}

} // Ir
} // Il
