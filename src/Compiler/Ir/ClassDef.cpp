#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- ClassDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ClassDef.h"

#include "./NamespaceBody.h"
#include "./NamespaceDef.h"
#include "./TypeParamDef.h"

#include "../Functor.h"

#include "../../Il/Ir/Operands/Class.h"
#include "../../Il/Ir/Operands/NameRef.h"
#include "../../Il/Ir/Operands/Type.h"

namespace Compiler {

// ctor
ClassDef::ClassDef(
    NamespaceBody& namespace_body,
    ClassOrNamespaceDef& owner,
    int const iModifiers,
    const Name& name,
    const SourceInfo& source_info)
    : Base(owner, iModifiers, name, source_info),
      namespace_body_(namespace_body) {}

// [A]
void ClassDef::AddBaseSpec(const Type& type) {
  base_specs_.Add(&type);
}

/// <summary>
///  Caller must check multiple occurence of type parameter name.
/// </summary>
void ClassDef::AddTypeParam(TypeParamDef& type_param) {
  ASSERT(!Find(type_param.name()));
  type_param.set_owner(*this);
  type_params_.Add(&type_param);
}

// [C]
int ClassDef::ComputeHashCode() const {
  return Common::ComputeHashCode(name().ComputeHashCode(), 'c');
}

// [G]
Class& ClassDef::GetClass() const {
  ASSERT(IsRealized());
  return *operand().StaticCast<Class>();
}

// [I]
bool ClassDef::IsClass() const {
  return (GetModifiers() & Modifier_Virtual) == 0;
}

bool ClassDef::IsInterface() const {
  return (GetModifiers() & Modifier_Mask_ClassVariation)
      == Modifier_ClassVariation_Interface;
}

bool ClassDef::IsStruct() const {
  return (GetModifiers() & Modifier_Mask_ClassVariation)
      == Modifier_ClassVariation_Struct;
}

// [S]
void ClassDef::SetClass() {
  ASSERT(!IsRealized());

  auto& outer_def = owner();
  auto& outer = outer_def.Is<NamespaceDef>()
      ? *outer_def.StaticCast<NamespaceDef>()->namespaze()
            .StaticCast<ClassOrNamespace>()
      : *outer_def.StaticCast<ClassDef>()->GetClass()
            .StaticCast<ClassOrNamespace>();

  ArrayList_<const TypeParam*> type_params;
  foreach (ClassDef::EnumTypeParam, oEnum, *this) {
    type_params.Add(&oEnum.Get()->type_param());
  }

  RealizeInternal(type_params.IsEmpty()
    ? *new Class(
        outer,
        modifiers(),
        name())
    : *new GenericClass(
        outer,
        modifiers(),
        name(),
        type_params));
}

// [T]
String ClassDef::ToString() const {
  Stack_<const NameDef*> stack;
  for (
      const NameDef* runner = this;
      !runner->IsGlobalNamespaceDef();
      runner = &runner->owner()) {
    stack.Push(runner);
  }

  ASSERT(!stack.IsEmpty());

  StringBuilder builder;
  const char* dot = "";
  while (!stack.IsEmpty()) {
    builder.Append(dot);
    dot = ".";
    builder.Append(stack.Pop()->name());
  }

  if (!type_params_.IsEmpty()) {
    builder.Append('<');
    const char* comma = "";
    foreach (EnumTypeParam, type_params, *this) {
      builder.Append(comma);
      comma = ", ";
      builder.Append(type_params.Get()->ToString());
    }
    builder.Append('>');
  }
  return builder.ToString();
}

} // Compiler
