#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - BoolOutput
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./BoolOutput.h"

#include "./PrimitiveType.h"

namespace Il {
namespace Ir {

BoolOutput::BoolOutput() {}

// [G]
const Type& BoolOutput::GetTy() const { return *Ty_Bool; }


String BoolOutput::ToString() const {
  if (this == False) {
    return "%false";
  }

  if (this == True) {
    return "%true";
  }

  return Base::ToString();
}

} // Ir
} // Il
