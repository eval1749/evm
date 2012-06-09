#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - VoidOutput
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./VoidOutput.h"

#include "./PrimitiveType.h"

namespace Il {
namespace Ir {

VoidOutput::VoidOutput() {
  ASSERT(Void == nullptr);
}

const Type& VoidOutput::GetTy() const { return *Ty_Void; }

String VoidOutput::ToString() const {
  return this == Void ? "%void" : "%uselss";
}

} // Ir
} // Il
