#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Instructions - CopyInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Physical.h"

namespace Il {
namespace Cg {

Physical::Physical(const RegDesc& reg_desc)
    : reg_desc_(reg_desc),
      use_count_(0) {}

} // Cg
} // Il
