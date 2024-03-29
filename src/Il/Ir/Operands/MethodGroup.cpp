#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - MethodGroup
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MethodGroup.h"

#include "./Class.h"
#include "../Name.h"

namespace Il {
namespace Ir {

MethodGroup::MethodGroup(Class& owner_class, const Name& name)
    : name_(name), owner_class_(owner_class) {
}

// [A]
void MethodGroup::Add(Method& method) {
  ASSERT(method.GetMethodGroup() == this);
  methods_.Append(&method);
} // Add

// [F]
Method* MethodGroup::Find(const FunctionType& type) const {
  for (auto& method: methods()) {
    if (method.function_type() == type)
      return &method;
  }
  return nullptr;
} // Find

// [G]
Namespace& MethodGroup::GetNamespace() const {
  return *owner_class_.GetNamespace();
} // GetNamespace

// [T]
String MethodGroup::ToString() const {
  return String::Format("%s.%s", owner_class_, name_);
}

} // Ir
} // Il
