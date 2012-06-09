#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - CopyInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FunLit.h"

namespace Il {
namespace Cg {

String FunLit::ToString() const {
  return String::Format("FunLit(%s)", value_);
}

} // Cg
} // Il
