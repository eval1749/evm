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

long TypeParam::s_lIndex;

// ctor
TypeParam::TypeParam(const Name& name)
  : m_fNewable(false),
    m_iIndex(static_cast<int>(::InterlockedIncrement(&s_lIndex))),
    name_(name),
    m_pOwner(nullptr) {}

// [B]
void TypeParam::BindTo(Operand* const pOwner) {
  ASSERT(pOwner != nullptr);
  ASSERT(m_pOwner == nullptr);
  m_pOwner = pOwner;
}

// [C]
int TypeParam::ComputeHashCode() const {
  auto const hash_code = name().ComputeHashCode();
  return Common::ComputeHashCode(m_iIndex, hash_code);
}

const Type& TypeParam::Construct(const TypeArgs& type_args) const {
  ASSERT(IsBound());
  auto& typaram = *this;
  auto const type = type_args.Get(typaram);
  return type ? *type : typaram;
}

// [G]
Operand* TypeParam::GetOwner() const {
  ASSERT(m_pOwner != nullptr);
  return m_pOwner;
}

GenericClass* TypeParam::GetOwnerClass() const {
  return GetOwner()->StaticCast<GenericClass>();
}

GenericMethod* TypeParam::GetOwnerMethod() const {
  return GetOwner()->StaticCast<GenericMethod>();
}

// [I]
Subtype TypeParam::IsSubtypeOf(const Type& r) const {
  if (*this == r || r == *Ty_Object) {
    return Subtype_Yes;
  }

  if (auto const that = r.DynamicCast<TypeParam>()) {
      return m_pOwner == that->m_pOwner && name_ == that->name_
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
