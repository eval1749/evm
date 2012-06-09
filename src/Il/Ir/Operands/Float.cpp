#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Operands - Float
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Float.h"
#include "./Variable.h"

namespace Il {
namespace Ir {

// ctor
Float::Float() :
    Base(RegClass_Fpr) {}

Float::Float(Variable* const var) 
    : Base(RegClass_Fpr) {
  ASSERT(var != nullptr);
  SetVariable(*var);
}

} // Ir
} // Il
