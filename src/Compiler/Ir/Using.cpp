#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Using
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Using.h"

namespace Compiler {

// ctor
Using::Using(NameRef* const name_ref)
    : Base(name_ref->GetSourceInfo()),
      class_or_namespace_(nullptr),
      name_ref_(name_ref) {
} // Using

} // Compiler
