#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- MethodDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MethodDef.h"

#include "./ClassDef.h"
#include "./MethodGroupDef.h"
#include "./TypeParamDef.h"

#include "../Functor.h"

#include "../../Common/LocalMemoryZone.h"

#include "../../Il/Ir/Module.h"

namespace Compiler {

using namespace Il::Ir;

// ctor
MethodDef::MethodDef(
    NamespaceBody& namespace_body,
    MethodGroupDef& method_group_def,
    int const modifiers,
    const Type& return_type,
    const NameRef& qualified_name)
    : Base(
          method_group_def.owner_class_def(),
          method_group_def.name(),
          qualified_name.source_info()),
      WithModifiers(modifiers),
      module_(*new Module()),
      function_(nullptr),
      index_(method_group_def.CountMethods()),
      method_group_(method_group_def),
      namespace_body_(namespace_body),
      qualified_name_(qualified_name),
      return_type_(&return_type),
      this_operand_((modifiers & Modifier_Static) == 0
          ? const_cast<Register&>(module_.NewRegister())
          : static_cast<Output&>(*Void)) {}

MethodDef::~MethodDef() {
  delete &module_;
}

/// <summary>
///     Returns a IL Method object compiled from this method definition.
///     //Note: You can't use this method until you call SetMethod.
/// </summary>
Method& MethodDef::method() const {
  return *operand().StaticCast<Method>();
}

ClassDef& MethodDef::owner_class_def() const {
  return *method_group_.owner().StaticCast<ClassDef>();
}

Collection_<TypeParamDef*> MethodDef::type_raram_defs() const {
  return Collection_<TypeParamDef*>(type_params_);
}

void MethodDef::set_function(Function& fun) {
  ASSERT(function_ == nullptr);

  function_ = &fun;

  if (auto const pRd = this_operand_.DynamicCast<Register>()) {
      auto const pPrologueI = fun.GetPrologueI();
      auto& zone = function_->module().zone();
      pPrologueI->GetBB()->AppendI(
          *new(zone) SelectI(
              owner_class_def().GetClass(),
              *pRd,
              *pPrologueI->GetVd(),
              0));
  }
}

// [A]
void MethodDef::Add(MethodParamDef& param_def) {
  AddParam(param_def);
}

void MethodDef::Add(VarDef& var) {
  ASSERT(var.owner() == *this);
  vars_.Append(&var);
}

/// <summary>
///  Caller must check multiple occurence of type parameter name.
/// </summary>
void MethodDef::Add(TypeParamDef& typaram_def) {
  Base::Add(typaram_def);
  type_params_.Add(&typaram_def);

  if (auto const unresolved = return_type_->DynamicCast<UnresolvedType>()) {
    if (unresolved->name_ref().name() == typaram_def.name()) {
      return_type_ = &typaram_def.type_param();
    }
  }
}

// [R]
void MethodDef::Realize(Method& method) {
  RealizeInternal(method);
  return_type_ = &method.return_type();
}

// [T]
String MethodDef::ToString() const {
  StringBuilder builder;
  builder.AppendFormat("%s %s(", *return_type_, name());
  const char* comma = "";
  foreach (EnumParam, params, *this) {
    auto& param = *params.Get();
    builder.Append(comma);
    comma = ", ";
    builder.AppendFormat("%s %s", param.GetTy(), param.name());
  }
  builder.Append(')');
  return builder.ToString();
}

} // Compiler
