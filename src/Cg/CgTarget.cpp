#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Functor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

namespace Il {
namespace Cg {

static CgTarget* s_target;

CgTarget& LibExport CgTarget::Get() {
  ASSERT(s_target != nullptr);
  return *s_target;
}

void LibExport CgTarget::Set(CgTarget& target) {
  ASSERT(s_target == nullptr);
  s_target = &target;
}

} // Cg
} // Il
