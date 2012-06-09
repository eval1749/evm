#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - RetI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./RetI.h"

#include "./ValuesI.h"

#include "../Operands/PrimitiveType.h"
#include "../Operands/VoidOutput.h"

namespace Il {
namespace Ir {

// ctor
RetI::RetI() : Base(Ty_Void, Void) {}

RetI::RetI(const Operand& sx) : Base(Ty_Void, Void) {
  AppendOperand(sx);
}

} // Ir
} // Il
