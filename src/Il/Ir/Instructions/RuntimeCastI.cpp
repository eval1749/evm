#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - RuntimeCastI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./RuntimeCastI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

RuntimeCastI::RuntimeCastI() : Base(Ty_Void, Void) {}

RuntimeCastI::RuntimeCastI(
    const Type& outy,
    const SsaOutput& out,
    const Operand& s0)
    : Base(outy, out) {
  AppendOperand(s0);
}

} // Ir
} // Il
