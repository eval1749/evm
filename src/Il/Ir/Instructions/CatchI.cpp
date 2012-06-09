#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - CatchI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CatchI.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
CatchI::CatchI() : Base(Ty_Void, Void) {}

CatchI::CatchI(
    const FrameReg& out,
    const Operand& s0,
    const BBlock& bblock)
    : Base(Ty_Void, Void) {
  AppendOperand(out);
  AppendOperand(s0);
  AppendOperand(bblock.label());
}

CatchI::CatchI(
    const FrameReg* const out,
    const Operand* const s0,
    const BBlock* const bblock)
    : Base(Ty_Void, Void) {
  AppendOperand(out);
  AppendOperand(s0);
  AppendOperand(bblock->label());
}

} // Ir
} // Il
