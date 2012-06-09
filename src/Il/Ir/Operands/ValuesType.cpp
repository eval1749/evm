#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Type
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ValuesType.h"

#include "./Class.h"
#include "./Namespace.h"
#include "./ValuesTypeBuilder.h"

namespace Il {
namespace Ir {

namespace {
struct MapKey {
  private: const Type::Collection* types_;

  // ctor
  public: MapKey(const Type::Collection* types = nullptr) : types_(types) {}
  public: MapKey(const Type::Collection& r) : types_(&r) {}

  public: bool operator==(const MapKey& r) {
    return Equals(r);
  }

  public: int ComputeHashCode() const {
    ASSERT(types_ != nullptr);
    auto iHashCode = types_->Count();
    foreach (Type::Collection::Enum, oEnum, *types_) {
      iHashCode = Common::ComputeHashCode(
          oEnum.Get()->ComputeHashCode(),
          iHashCode);
    }
    return iHashCode;
  }

  public: bool Equals(const MapKey another) const {
    ASSERT(types_ != nullptr);
    ASSERT(another.types_ != nullptr);
    return *types_ == *another.types_;
  }
}; // MapKey

inline int ComputeHashCode(const MapKey& r) {
  return r.ComputeHashCode();
}

class ValuesTypeMap
    : public HashMap_<MapKey, ValuesType*>,
      public Lockable {
  public: ValuesTypeMap() {}
  DISALLOW_COPY_AND_ASSIGN(ValuesTypeMap);
};

static ValuesTypeMap* s_values_type_map;
} // namespace

// ctor
ValuesType::ValuesType(const Type::Collection& types)
    : types_(types) {}

// [C]
const Type& ValuesType::ComputeBoundType() const {
  ValuesTypeBuilder builder(Count());
  foreach (Enum, en, this) {
    builder.Append(en.Get().ComputeBoundType());
  }
  return builder.GetValuesType();
}

int ValuesType::ComputeHashCode() const {
  auto hash_code = static_cast<int>('V');
  foreach (Enum, types, this) {
    auto& ty = *types;
    hash_code = Common::ComputeHashCode(ty.ComputeHashCode(), hash_code);
  }
  return hash_code;
}

const ArrayType* ValuesType::ComputeRestType() const {
  auto const count = types_.Count();
  if (count == 0) {
    return nullptr;
  }

  auto& last_ty = Get(count - 1);
  return last_ty.DynamicCast<ArrayType>();
}

const Type& ValuesType::Construct(const TypeArgs& type_args) const {
  ValuesTypeBuilder builder(Count());
  foreach (ValuesType::Enum, types, this) {
    auto& type = *types;
    builder.Append(type.Construct(type_args));
  }
  return builder.GetValuesType();
}

int ValuesType::Count() const {
  return types_.Count();
}

// [G]
const Type& ValuesType::Get(int const index) const {
  return *types_.Get(index);
}

// [I]
const ValuesType& ValuesType::Intern() {
  return Intern(Type::Collection());
}

const ValuesType& ValuesType::Intern(const Type::Collection& types) {
  if (!s_values_type_map) {
    s_values_type_map = new ValuesTypeMap();
  }

  ScopedLock lock(*s_values_type_map);

  if (auto const present = s_values_type_map->Get(types)) {
    ASSERT(present->Count() == types.Count());
    return *present;
  }

  auto& newty = *new ValuesType(types);
  s_values_type_map->Add(&newty.types_, &newty);
  return newty;
}

const ValuesType& ValuesType::Intern(const Type& ty1) {
  ASSERT(!ty1.Is<ValuesType>());
  return Intern(CollectionV_<const Type*>(&ty1));
}

const ValuesType& ValuesType::Intern(const Type& ty1, const Type& ty2) {
  return Intern(CollectionV_<const Type*>(&ty1, &ty2));
}

bool ValuesType::IsBound() const {
  foreach (ValuesType::Enum, oEnum, this) {
    if (!oEnum.Get().IsBound()) {
      return false;
    }
  }
  return true;
}

Subtype ValuesType::IsSubtypeOf(const Type& r) const {
  if (*this == r) {
      return Subtype_Yes;
  }

  auto const that = r.DynamicCast<ValuesType>();
  if (that == nullptr) {
      return Subtype_No;
  }

  if (Count() != that->Count()) {
      return Subtype_No;
  }

  auto eTotalSubtype = Subtype_Yes;
  ValuesType::Enum oEnumThat(that);
  foreach (ValuesType::Enum, oEnumThis, this) {
      switch (oEnumThis.Get().IsSubtypeOf(oEnumThat.Get())) {
      case Subtype_No:
          eTotalSubtype = Subtype_No;
          break;

      case Subtype_Yes:
          break;

      case Subtype_Unknown:
          if (eTotalSubtype == Subtype_Yes) {
              eTotalSubtype = Subtype_Unknown;
          }
          break;

      default:
          CAN_NOT_HAPPEN();
      }
      oEnumThat.Next();
  }

  return eTotalSubtype;
}

// [M]
void LibExport ValuesType::MapAll(Functor& fn) {
  if (!s_values_type_map) return;
  ScopedLock lock(*s_values_type_map);
  foreach (ValuesTypeMap::Enum, entries, s_values_type_map) {
    entries.Get().GetValue()->Apply(&fn);
  }
}

// [T]
String ValuesType::ToString() const {
  StringBuilder builder;
  builder.Append("(");
  const char* comma = "";
  foreach (Enum, elems, this) {
    builder.Append(comma);
    comma = ", ";
    builder.Append(elems.Get().ToString());
  }
  builder.Append(")");
  return builder.ToString();
}

// [U]
void ValuesType::Unintern() {
  ScopedLock lock(*s_values_type_map);
  if (!s_values_type_map->Remove(MapKey(types_))) {
    CAN_NOT_HAPPEN();
  }
}

} // Ir
} // Il
