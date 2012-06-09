#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- FieldDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FieldDef.h"

#include "./ClassDef.h"
//#include "../Functor.h"

namespace Compiler {

// ctor
FieldDef::FieldDef(
    ClassDef& owner,
    int const modifiers,
    const Type& type,
    const Name& name,
    const SourceInfo& source_info) 
    : Base(owner, name, source_info),
      modifiers_(modifiers),
      type_(type) {
  ASSERT(type != *Ty_Void);
}

} // Compiler
