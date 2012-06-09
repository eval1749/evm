#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - String
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NamespaceDef.h"

#include "./ClassDef.h"
#include "./TypeDef.h"
#include "../Functor.h"

#include "../CompileSession.h"

#include "../../Common/GlobalMemoryZone.h"
#include "../../Common/String.h"
#include "../../Common/StringBuilder.h"

#include "../../Il/Ir/Operands.h"

namespace Compiler {

namespace {

Namespace* InternNamespace(Namespace* outer_ns, const Name& name) {
  if (!outer_ns) {
    return nullptr;
  }

  if (auto const present = outer_ns->Find(name)) {
    return present->DynamicCast<Namespace>();
  }

  return new Namespace(*outer_ns, name);
}

} // namespace

// ctor
NamespaceDef::NamespaceDef(
    NamespaceDef& outer,
    const Name& name,
    const SourceInfo& source_info)
    : Base(outer, name, source_info),
      real_namespace_(InternNamespace(outer.real_namespace_, name)) {
}

// Ctor for global compilation environment.
NamespaceDef::NamespaceDef(Namespace& ns, Namespace& real_ns)
    : ALLOW_THIS_IN_INITIALIZER_LIST(
        Base(*this, ns.name(), SourceInfo())),
      real_namespace_(&real_ns) {
  Realize(ns);
}

// properties
Namespace& NamespaceDef::namespaze() const {
  ASSERT(IsRealized());
  return *operand().StaticCast<Namespace>();
}

NamespaceDef* NamespaceDef::outer() const {
  return &owner() == this
      ? nullptr
      : owner().StaticCast<NamespaceDef>();
}

// [C]
// Create compilation global namespace for compilation. We use compilation
// global namespace for isolating compilation environment and runtime
// environment.
NamespaceDef& NamespaceDef::CreateGlobalNamespaceDef(MemoryZone& zone) {
 return *new(zone) NamespaceDef(
    Namespace::CreateGlobalNamespace(),
    *Namespace::Global);
}

// [F]
ClassOrNamespace* NamespaceDef::FindObject(const Name& name) const {
  if (auto const present = namespaze().Find(name)) {
    return present->StaticCast<ClassOrNamespace>();
  }

  return real_namespace_
      ? real_namespace_->Find(name)->DynamicCast<ClassOrNamespace>()
      : nullptr;
}

// [T]
String NamespaceDef::ToString() const {
  Stack_<const NameDef*> stack;
  for (
      auto runner = this;
      !runner->IsGlobalNamespaceDef();
      runner = runner->owner().StaticCast<NamespaceDef>()) {
    stack.Push(runner);
  }

  if (stack.IsEmpty()) {
    return "(global namespace)";
  }

  StringBuilder builder;
  const char* dot = "";
  while (!stack.IsEmpty()) {
    builder.Append(dot);
    dot = ".";
    builder.Append(stack.Pop()->name());
  }

  return builder.ToString();
}

} // Compiler
