#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - LogXorI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LogXorI.h"

#include "../Operands/PrimitiveType.h"
#include "../Operands/VoidOutput.h"

namespace Il {
namespace Ir {

LogXorI::LogXorI() : Base(Ty_Void, Void) {}

LogXorI::LogXorI(
    const Type& outy,
    const Register& out,
    const Operand& src0,
    const Operand& src1)
    : Base(&outy, &out) {
  AppendOperand(src0);
  AppendOperand(src1);
}

LogXorI::LogXorI(
    const Type* const outy,
    const Register* const out,
    const Operand* const src0,
    const Operand* const src1)
    : Base(outy, out) {
  AppendOperand(src0);
  AppendOperand(src1);
}

} // Ir
} // Il
