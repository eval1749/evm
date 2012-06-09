#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - String
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NamespaceBody.h"

#include "./AliasDef.h"
#include "./NamespaceDef.h"
#include "./UsingNamespace.h"
#include "../Functor.h"

#include "../../Common/GlobalMemoryZone.h"

namespace Compiler {

// ctor
NamespaceBody::NamespaceBody(
    CompilationUnit& compliation_unit,
    NamespaceBody* const outer,
    NamespaceDef& namespace_def,
    const SourceInfo* const source_info) :
        Base(source_info),
        // TODO(yosi) 2011-01-04 We should use parser local memory zone for
        // NamespaceBody::name_table
        name_table_(GlobalMemoryZone::Get()),
        compilation_unit_(compliation_unit),
        namespace_def_(namespace_def),
        outer_(outer) {
  if (outer_ != nullptr) {
    outer_->members_.Append(this);
  }
} // NamespaceDef

// [A]
void NamespaceBody::Add(NameDef& name_def) {
  members_.Append(&name_def);
  name_table_.Add(&name_def.name(), &name_def);
  if (!name_def.Is<AliasDef>()) {
    namespace_def_.Add(name_def);
  }
} // Add

void NamespaceBody::Add(UsingNamespace& uzing) {
  // TODO 2010-12-30 yosi@msn.com Check duplicate using namespace.
  usings_.Append(&uzing);
} // Add

// [F]
NameDef* NamespaceBody::Find(const Name& name) const {
  return name_table_.Get(&name);
}

} // Compiler
