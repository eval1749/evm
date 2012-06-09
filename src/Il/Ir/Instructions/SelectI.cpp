#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - SelectI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./SelectI.h"

#include "./ValuesI.h"

#include "../Operands.h"

#include "../../../Common/Int32.h"

namespace Il {
namespace Ir {

SelectI::SelectI() : Base(Ty_Void, Void) {}

SelectI::SelectI(
    const Type& outy,
    const Register& r0,
    const Values& v1,
    int const iNth) 
    : Base(outy, r0) {
  AppendOperand(v1);
  AppendOperand(new Int32Literal(iNth));
} // SelectI

} // Ir
} // Il
