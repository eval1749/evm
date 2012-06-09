#include "precomp.h"
// @(#)$Id$
//
// Cg Test Operands
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

namespace Il {
namespace Cg {

String StackSlot::ToString() const {
  return String::Format("%stack[%d:%d]", offset(), size());
}

} // Cg
} // Il
