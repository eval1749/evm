#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- AliasDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AliasDef.h"

#include "./NamespaceDef.h"
#include "../Functor.h"

namespace Compiler {

AliasDef::AliasDef(
    NamespaceDef* owner,
    Name* const name,
    NameRef* const name_ref,
    const SourceInfo* source_info)
    : Base(owner, name, source_info),
      name_ref_(name_ref) {
  ASSERT(name_ref_ != nullptr);
} // AliasDef

} // Compiler
