#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- TypeParam
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeParam.h"

#include "./GenericClass.h"
#include "./GenericMethod.h"
#include "./TypeArgs.h"
#include "../Name.h"

namespace Il {
namespace Ir {

// ctor
TypeParam::TypeParam(const Name& name)
  : ctor_constraint_(NotNewable),
    name_(name),
    owner_(nullptr),
    realized_(false) {}

// properties
bool TypeParam::is_newable() const {
  ASSERT(is_realized());
  return ctor_constraint_ == Newable;
}

const Operand& TypeParam::owner() const {
  ASSERT(!!owner_);
  return *owner_;
}

// [B]
void TypeParam::BindTo(const Operand& owner) {
  ASSERT(owner.Is<Class>() || owner.Is<Method>());
  ASSERT(!owner_);
  owner_ = &owner;
}

// [C]
int TypeParam::ComputeHashCode() const {
  auto const hash_code = name().ComputeHashCode();
  return Common::ComputeHashCode('T', hash_code);
}

const Type& TypeParam::Construct(const TypeArgs& type_args) const {
  ASSERT(IsBound());
  auto& typaram = *this;
  auto const type = type_args.Get(typaram);
  return type ? *type : typaram;
}

// [I]
Subtype TypeParam::IsSubtypeOf(const Type& r) const {
  if (*this == r || r == *Ty_Object) {
    return Subtype_Yes;
  }

  if (!is_realized()) {
    return Subtype_Unknown;
  }

  if (auto const that = r.DynamicCast<TypeParam>()) {
    foreach (EnumConstraint, it, *that) {
      auto const is_subtype_of = IsSubtypeOf(**it);
      if (is_subtype_of != Subtype_Yes) {
        return is_subtype_of;
      }
    }
    return Subtype_Yes;
  }

  foreach (EnumConstraint, it, *this) {
    auto const is_subtype_of = it->IsSubtypeOf(r);
    if (is_subtype_of != Subtype_Yes) {
      return is_subtype_of;
    }
  }

  return Subtype_Yes;
}

// [R]
void TypeParam::RealizeTypeParam(
    const Constraints& class_constraints,
    ConstructorConstraint ctor_constraint) {
  ASSERT(!is_realized());
  class_constraints_ = class_constraints;
  ctor_constraint_ = ctor_constraint;
  realized_ = true;
}

// [T]
String TypeParam::ToString() const {
  return name_;
}

} // Ir
} // iL
