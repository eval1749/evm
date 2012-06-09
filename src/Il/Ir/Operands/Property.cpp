#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Type
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Property.h"

#include "./Class.h"
#include "./Method.h"
#include "../Name.h"

namespace Il {
namespace Ir {

// ctor
Property::Property(
    const Class& owner_class,
    int const modifiers,
    const Type& property_type,
    const Name& name,
    const SourceInfo* source_info)
    : WithModifiers(modifiers),
      WithSourceInfo(source_info),
      name_(name),
      owner_class_(owner_class),
      property_type_(property_type) {
  ASSERT((modifiers & ~PropertyModifiers) == 0);
}

// [A]
void Property::Add(const Name& name, const Method& method) {
  ASSERT(!member_map_.Contains(&name));
  ASSERT((method.modifiers() & Modifier_SpecialName) != 0);
  ASSERT((method.modifiers() & ~AccessorModifiers) == 0);
  member_map_.Add(&name, &method);
}

// [G]
Method* Property::Get(const Name& name) const {
  return const_cast<Method*>(member_map_.Get(&name));
}

// [T]
String Property::ToString() const {
  return String::Format("%s %s.%s", property_type_, owner_class_, name_);
}

} // Ir
} // Il
