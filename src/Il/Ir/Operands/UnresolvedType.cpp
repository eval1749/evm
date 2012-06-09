#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- UnresolvedType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./UnresolvedType.h"

#include "./NameRef.h"

namespace Il {
namespace Ir {

// ctor
UnresolvedType::UnresolvedType(const NameRef& name_ref)
    : name_ref_(name_ref),
      resolved_type_(nullptr) {}

const Type& UnresolvedType::resolved_type() const {
  ASSERT(resolved_type_ != nullptr);
  return *resolved_type_;
}

const SourceInfo& UnresolvedType::source_info() const {
  return name_ref_.source_info();
}

// [B]
void UnresolvedType::Bind(const Type& ty) {
  ASSERT(!IsBound());
  resolved_type_ = &ty;
}

// [C]
const Type& UnresolvedType::ComputeBoundType() const {
  return resolved_type_ ? *resolved_type_ : *this;
}

int UnresolvedType::ComputeHashCode() const {
  return Common::ComputeHashCode(name_ref_.ComputeHashCode(), 'U');
}

const Type& UnresolvedType::Construct(const TypeArgs&) const {
  return *const_cast<UnresolvedType*>(this);
}

// [I]
bool UnresolvedType::IsBound() const {
  return resolved_type_ != nullptr;
}

Subtype UnresolvedType::IsSubtypeOf(const Type& r) const {
  return *this == r ? Subtype_Yes : Subtype_Unknown;
}

// [T]
String UnresolvedType::ToString() const {
  return IsBound()
    ? String::Format("?%s=%s", name_ref_, resolved_type_)
    : String::Format("?%s", name_ref_);
}

} // Ir
} // Il
