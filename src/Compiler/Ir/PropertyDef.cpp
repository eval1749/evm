#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Property
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./PropertyDef.h"

#include "./ClassDef.h"
#include "../Functor.h"

namespace Compiler {

PropertyDef::PropertyDef(
    NamespaceBody& namespace_body,
    ClassDef& owner_class,
    int const modifiers,
    const Type& property_type,
    const NameRef& qualified_name)
    : Base(
          owner_class,
          Name::Intern(qualified_name.ToString()),
          qualified_name.source_info()),
      WithModifiers(modifiers),
      namespace_body_(namespace_body),
      property_type_(property_type),
      qualified_name_(qualified_name) {}

// [A]
void PropertyDef::Add(MethodParamDef& param_def) {
  AddParam(param_def);
}

void PropertyDef::AddMember(const Name& name, const MethodDef& method_def) {
  member_map_.Add(&name, &method_def);
}

// [F]
MethodDef* PropertyDef::FindMember(const Name& name) const {
  return const_cast<MethodDef*>(member_map_.Get(&name));
}

} // Compiler
