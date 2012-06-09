#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- PointerType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./PointerType.h"

#include "./Class.h"


namespace Il {
namespace Ir {

namespace {
// Map pointee type to PointerType object.
class PointerTypeMap
    : public HashMap_<const Type*, PointerType*>,
      public Lockable {
  public: PointerTypeMap() {}
  DISALLOW_COPY_AND_ASSIGN(PointerTypeMap);
};

static PointerTypeMap* s_pointer_type_map;
} // namespace

// ctor
PointerType::PointerType(const Type& pointee_type)
    : pointee_type_(pointee_type) {}

// [C]
const Type& PointerType::ComputeBoundType() const {
  return PointerType::Intern(pointee_type_.ComputeBoundType());
}

int PointerType::ComputeHashCode() const {
  return Common::ComputeHashCode(pointee_type_.ComputeHashCode(), 'P');
}

const Type& PointerType::Construct(const TypeArgs& type_args) const {
  return PointerType::Intern(pointee_type_.Construct(type_args));
}

// [I]
const PointerType& PointerType::Intern(const Type& pointee_type) {
  if (!s_pointer_type_map) {
    s_pointer_type_map = new PointerTypeMap();
  }

  ScopedLock lock(*s_pointer_type_map);

  if (auto const present = s_pointer_type_map->Get(&pointee_type)) {
    return *present;
  }

  auto const newty = new PointerType(pointee_type);
  s_pointer_type_map->Add(&pointee_type, newty);
  return *newty;
} // Intern

bool PointerType::IsBound() const {
  return pointee_type_.IsBound();
}

Subtype PointerType::IsSubtypeOf(const Type& r) const {
  if (*this == r || r == *Ty_Object) {
    return Subtype_Yes;
  }

  if (auto const that = r.DynamicCast<PointerType>()) {
    return pointee_type_.IsSubtypeOf(that->pointee_type());
  }

  return Subtype_No;
}

// [M]
void LibExport PointerType::MapAll(Functor& fn) {
  if (!s_pointer_type_map) return;
  ScopedLock lock(*s_pointer_type_map);
  foreach (PointerTypeMap::Enum, entries, s_pointer_type_map) {
    entries.Get().GetValue()->Apply(&fn);
  }
}

// [T]
String PointerType::ToString() const {
  return String::Format("*%s", pointee_type_);
}

// [U]
void PointerType::Unintern() {
  ScopedLock lock(*s_pointer_type_map);
  if (!s_pointer_type_map->Remove(&pointee_type_)) {
    CAN_NOT_HAPPEN();
  }
}

} // Ir
} // Il
