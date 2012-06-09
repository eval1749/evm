#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Output
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Output.h"

#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
Output::Output() : name_(0) {}

// [G]
const Type& Output::GetTy() const { return *Ty_Object; }

// [T]
String Output::ToString() const {
  return String::Format("%%%c%d", GetPrefixChar(), name_);
}

} // Ir
} // Il
