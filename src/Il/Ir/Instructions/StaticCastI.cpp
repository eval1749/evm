#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - StaticCastI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./StaticCastI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

StaticCastI::StaticCastI() : Base(Ty_Void, Void) {}

StaticCastI::StaticCastI(
    const Type& outy,
    const SsaOutput& out,
    const Operand& src)
    : Base(outy, out) {
  AppendOperand(src);
}

} // Ir
} // Il
