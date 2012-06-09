#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- IrIdentifier
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NameScope.h"

#include "../../Common/GlobalMemoryZone.h"

#include "../Ir/NameDef.h"

namespace Compiler {

NameScope::NameScope(MemoryZone& zone, NameScope* const outer)
    : name_table_(zone),
      outer_(outer) {
  // outer_ can be null.
}

void NameScope::Add(NameDef& name_def) {
  name_table_.Add(&name_def.name(), &name_def);
}

NameDef* NameScope::Find(const Name& name) const{
  foreach (EnumAncestor, ancestors, this) {
    auto const ancestor = ancestors.Get();
    if (auto const pPresent = ancestor->name_table_.Get(&name)) {
      return pPresent;
    } // if
  } // for
  return nullptr;
} // Find

} // Compiler
