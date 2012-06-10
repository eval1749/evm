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
  : name_(name),
    owner_(nullptr) {}

// properties
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

  if (auto const that = r.DynamicCast<TypeParam>()) {
      return owner_ == that->owner_ && name_ == that->name_
        ? Subtype_Yes
        : Subtype_No;
  }

  return Subtype_Unknown;
}

// [T]
String TypeParam::ToString() const {
  return name_;
}

} // Ir
} // iL
