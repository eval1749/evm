#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- ClassOrNamespace
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ClassOrNamespace.h"

#include "./ConstructedClass.h"
#include "./Namespace.h"
#include "../Name.h"

#include "../../../Common/GlobalMemoryZone.h"

#include "./TypeVar.h"

namespace Il {
namespace Ir {

// ctor
ClassOrNamespace::ClassOrNamespace(
  const ClassOrNamespace& outer,
  const Name& name)
  : name_(name),
    outer_(const_cast<ClassOrNamespace&>(outer)) {}

ClassOrNamespace::~ClassOrNamespace() {
  outer_.name_table_.Remove(&name_);
}

// [A]
void ClassOrNamespace::Add(const Name& name, Operand& operand) {
  ASSERT(!operand.Is<TypeVar>());
  ASSERT(!name_table_.Contains(&name));
  name_table_.Add(&name, &operand);
}

// [F]
Operand* ClassOrNamespace::Find(const Name& name) const {
  return name_table_.Get(&name);
}

// [T]
String ClassOrNamespace::ToString() const {
  if (IsGlobalNamespace()) {
    return "(GlobalNamespace)";
  }

  Stack_<const ClassOrNamespace*> spaces;
  foreach (EnumOuterAndSelf, outers, *this) {
    auto& outer = outers.Get();
    if (outer.IsGlobalNamespace()) {
      break;
    }
    spaces.Push(&outer);
  }

  StringBuilder builder;
  while (!spaces.IsEmpty()) {
    auto const space = spaces.Pop();
    builder.Append(space->name().ToString());
    if (!spaces.IsEmpty()) {
      builder.Append('.');
    }
  }
  return builder.ToString();
}

} // Ir
} // Il
