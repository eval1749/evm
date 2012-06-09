#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./GenCodeDesc.h"

namespace Ee {

CodeDesc::CodeDesc(
    const Function& fun,
    const Collection_<Annotation>& annotations,
    void* const code_vector,
    uint32 const code_size)
    : annotations_(annotations),
      code_size_(code_size),
      codes_(code_vector),
      function_(fun) {
  ASSERT(code_size> 0);
  ASSERT(code_vector != nullptr);
}

} // Ee
