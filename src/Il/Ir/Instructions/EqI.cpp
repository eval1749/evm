#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - EqI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./EqI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
EqI::EqI() : Base(Ty_Void, Void) {}

EqI::EqI(const BoolOutput& out, const Operand& s0, const Operand& s1)
    : Base(*Ty_Bool, out) {
  AppendOperand(s0);
  AppendOperand(s1);
}

// TODO(yosi) 2012-02-06 Once Compiler::DEFOPERATOR_COMP is fixed, we can
// remove this method.
EqI::EqI(
    const BoolOutput* const out,
    const Operand* const s0,
    const Operand* const s1)
    : Base(Ty_Bool, out) {
  AppendOperand(s0);
  AppendOperand(s1);
}

} // Ir
} // Il
