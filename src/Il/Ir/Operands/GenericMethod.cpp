#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- GenericMethod
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./GenericMethod.h"

#include "./ConstructedMethod.h"

namespace Il {
namespace Ir {

// ctor
GenericMethod::GenericMethod(
    const MethodGroup& method_group,
    int const iModifiers,
    const FunctionType& function_type,
    const Collection_<const TypeParam*>& type_params)
    : Base(
            method_group,
            iModifiers,
            function_type),
        type_params_(type_params) {
  ASSERT(type_params.Count() > 0);
  foreach (TypeParams::Enum, params, type_params) {
    auto& type_param = *const_cast<TypeParam*>(params.Get());
    type_param.BindTo(this);
  } // for
} // GenericMethod

// [C]
ConstructedMethod& GenericMethod::Construct(
    const TypeArgs& type_args) const {
  if (auto const present = cons_method_map_.Get(&type_args)) {
    return *present;
  }

  auto& cons_method = *new ConstructedMethod(*this, type_args);
  cons_method_map_.Add(&cons_method.type_args(), &cons_method);
  return cons_method;
}

} // Ir
} // Il
