#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - String
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ClassOrNamespaceDef.h"

#include "../Functor.h"

namespace Compiler {

ClassOrNamespaceDef::ClassOrNamespaceDef(
    NameDef& owner,
    const Name& name,
    const SourceInfo& source_info)
    : Base(owner, name, source_info) {
}

// [A]
void ClassOrNamespaceDef::Add(NameDef& name_def) {
  ASSERT(&name_def.owner() == this);
  name_table_.Add(&name_def.name(), &name_def);
  name_defs_.Append(&name_def);
} // Add

// [F]
NameDef* ClassOrNamespaceDef::Find(const Name& name) const {
  return name_table_.Get(&name);
} // Find

} // Compiler
