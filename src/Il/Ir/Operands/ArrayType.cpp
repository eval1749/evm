#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- ArrayType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ArrayType.h"

#include "./Class.h"
#include "./ConstructedClass.h"
#include "./GenericClass.h"

namespace Il {
namespace Ir {

namespace {
struct MapKey {
  const Type* element_type_;
  int rank_;

  public: MapKey(const Type& element_type, int const rank)
      : element_type_(&element_type), rank_(rank) {}

  public: MapKey() : element_type_(nullptr), rank_(0) {}

  public: bool operator==(const MapKey& r) const { return Equals(r); }

  public: int ComputeHashCode() const {
    ASSERT(this != nullptr);
    return Common::ComputeHashCode(element_type_->ComputeHashCode(), rank_);
  }

  public: bool Equals(const MapKey& r) const {
    ASSERT(this != nullptr);
    return element_type_ == r.element_type_ && rank_ == r.rank_;
  }
}; // MapKey

inline int ComputeHashCode(const MapKey& r) {
  return r.ComputeHashCode();
}

class ArrayTypeMap
    : public HashMap_<MapKey, ArrayType*>,
      public Lockable {
  public: ArrayTypeMap() {}
  DISALLOW_COPY_AND_ASSIGN(ArrayTypeMap);
};

static ArrayTypeMap* s_array_type_map;
} // namespace

// ctor
ArrayType::ArrayType(const Type& element_type, int const rank)
    : element_type_(element_type),
      rank_(rank) {
  ASSERT(rank >= 0);
}

// [C]
const Type& ArrayType::ComputeBoundType() const {
  return ArrayType::Intern(element_type_.ComputeBoundType(), rank_);
}

int ArrayType::ComputeHashCode() const {
  return Common::ComputeHashCode(element_type_.ComputeHashCode(), 'A');
} // ComputeHashCode

const Type& ArrayType::Construct(const TypeArgs& type_args) const {
  return ArrayType::Intern(element_type_.Construct(type_args), rank_);
} // Construct

// [I]
const ArrayType& LibExport ArrayType::Intern(
    const Type& element_type,
    int const rank) {
  if (!s_array_type_map) {
    s_array_type_map = new ArrayTypeMap();
  }

  ScopedLock lock(*s_array_type_map);

  MapKey key(element_type, rank);
  if (auto const present = s_array_type_map->Get(key)) {
    return *present;
  }

  auto const newty = new ArrayType(element_type, rank);
  s_array_type_map->Add(key, newty);
  return *newty;
} // Intern

bool ArrayType::IsBound() const {
  return element_type().IsBound();
} // IsBound

Subtype ArrayType::IsSubtypeOf(const Type& r) const {
  if (*this == r || r == *Ty_Object) {
    return Subtype_Yes;
  }

  if (auto const that = r.DynamicCast<ArrayType>()) {
    if (rank() != that->rank()) {
      return Subtype_No;
    }
    return element_type().IsSubtypeOf(that->element_type());
  }

  if (auto const cclass = r.DynamicCast<ConstructedClass>()) {
    return Ty_Array->Construct(element_type_).IsSubtypeOf(*cclass);
  }

  return Subtype_No;
}

// [M]
void LibExport ArrayType::MapAll(Functor& fn) {
  if (!s_array_type_map) return;
  ScopedLock lock(*s_array_type_map);
  foreach (ArrayTypeMap::Enum, entries, s_array_type_map) {
    entries.Get().GetValue()->Apply(&fn);
  }
}

// [T]
String ArrayType::ToString() const {
  StringBuilder builder;
  builder.Append(element_type_.ToString());
  builder.Append('[');
  for (int i = 1; i < rank_; i++) {
    builder.Append(',');
  }
  builder.Append(']');
  return builder.ToString();
}

// [U]
void ArrayType::Unintern() {
  ScopedLock lock(*s_array_type_map);
  if (!s_array_type_map->Remove(MapKey(element_type_, rank_))) {
    CAN_NOT_HAPPEN();
  }
}

} // Ir
} // Il
