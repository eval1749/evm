#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- FunctionType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FunctionType.h"

#include "./Class.h"
#include "./ValuesTypeBuilder.h"

namespace Il {
namespace Ir {

namespace {
struct MapKey {
  const ValuesType* params_type_;
  const Type* return_type_;

  MapKey() : params_type_(nullptr), return_type_(nullptr) {}

  MapKey(const Type& rety, const ValuesType& paramsty)
      : params_type_(&paramsty), return_type_(&rety) {}

  bool operator==(const MapKey& r) const {
    return Equals(r);
  }

  int ComputeHashCode() const {
    ASSERT(params_type_ != nullptr);
    ASSERT(return_type_ != nullptr);
    return Common::ComputeHashCode(
              return_type_->ComputeHashCode(),
              params_type_->ComputeHashCode());
  }

  bool Equals(const MapKey& another) const {
    ASSERT(params_type_ != nullptr);
    ASSERT(return_type_ != nullptr);
    ASSERT(another.params_type_ != nullptr);
    ASSERT(another.return_type_ != nullptr);
    return *return_type_ == *another.return_type_
        && *params_type_ == *another.params_type_;
  }
};

inline int ComputeHashCode(const MapKey& r) {
  return r.ComputeHashCode();
}

class FunctionTypeMap
    : public HashMap_<MapKey, FunctionType*>, 
      public Lockable {
  public: FunctionTypeMap() : HashMap_(100) {}
  DISALLOW_COPY_AND_ASSIGN(FunctionTypeMap);
};

static FunctionTypeMap* s_funty_map;
} // namespace

// ctor
FunctionType::FunctionType(const Type& rety, const ValuesType& paramsty)
    : params_type_(paramsty),
      return_type_(rety) {}

// [C]
Type& FunctionType::ComputeBoundType() const {
  return FunctionType::Intern(
    return_type().ComputeBoundType(),
    *params_type().ComputeBoundType().StaticCast<ValuesType>());
}

int FunctionType::ComputeHashCode() const {
  auto hash_code = Common::ComputeHashCode(
    return_type_.ComputeHashCode(),
    'F');

  foreach (EnumParamType, oEnum, this) {
    hash_code = Common::ComputeHashCode(
        oEnum.Get().ComputeHashCode(),
        hash_code);
  }
  return hash_code;
}

const ArrayType* FunctionType::ComputeRestType() const {
  return params_type_.ComputeRestType();
}

const Type& FunctionType::Construct(const TypeArgs& type_args) const {
  return FunctionType::Intern(
      return_type_.Construct(type_args),
      *params_type_.Construct(type_args).StaticCast<ValuesType>());
}

int FunctionType::CountParams() const {
  return params_type_.Count();
}

// [I]
FunctionType& FunctionType::Intern(
    const Type& rety, 
    const ValuesType& paramsty) {
  if (!s_funty_map) {
      s_funty_map = new FunctionTypeMap();
  }

  ScopedLock lock(*s_funty_map);

  MapKey key(rety, paramsty);

  if (auto const presnet = s_funty_map->Get(key)) {
    return *presnet;
  }

  auto const newty = new FunctionType(rety, paramsty);
  s_funty_map->Add(key, newty);
  return *newty;
}

bool FunctionType::IsBound() const {
    return return_type_.IsBound() && params_type_.IsBound();
}

Subtype FunctionType::IsSubtypeOf(const Type& r) const {
  if (*this == r || r == *Ty_Object) {
    return Subtype_Yes;
  }

  // TODO 2011-01-05 yosi@msn.com We may want to have callable type.
  return Subtype_Unknown;
}

// [M]
void LibExport FunctionType::MapAll(Functor& functor) {
  if (!s_funty_map) return;
  ScopedLock lock(*s_funty_map);
  foreach (FunctionTypeMap::Enum, entries, s_funty_map) {
    entries.Get().GetValue()->Apply(&functor);
  }
}

// [T]
String FunctionType::ToString() const {
  return String::Format("%s%s", return_type_, params_type_);
}

// [U]
void FunctionType::Unintern() {
  ScopedLock lock(*s_funty_map);
  if (!s_funty_map->Remove(MapKey(return_type_, params_type_))) {
    CAN_NOT_HAPPEN();
  }
}

} // Ir
} // Il
