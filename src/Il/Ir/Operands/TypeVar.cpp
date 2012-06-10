#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Type
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeVar.h"

#include "./Class.h"
#include "./PrimitiveType.h"

namespace Il {
namespace Ir {

namespace {

// Compute intersection of types. If specified types are disjoin, this
// function returns Ty_Void as empty type set.
static const Type& TypeAnd(const Type& a, const Type& b) {
  ASSERT(a != *Ty_Void);
  ASSERT(b != *Ty_Void);

  auto const ab = a.IsSubtypeOf(b);
  auto const ba = b.IsSubtypeOf(a);

  if (ab == Subtype_Yes) {
    return a;
  }

  if (ba == Subtype_Yes) {
    return b;
  }

  return ab == Subtype_Unknown && ba == Subtype_Unknown
      ? *Ty_Object
      : *Ty_Void;
}

// Compute union of types. This function returns Ty_Void if specified types
// are disjoin. Caller should handle Ty_Void reutrn value specially.
static const Type& TypeOr(const Type& a, const Type& b) {
  ASSERT(a != *Ty_Void);
  ASSERT(b != *Ty_Void);
  if (a.IsSubtypeOf(b) == Subtype_Yes) {
    return b;
  }

  if (b.IsSubtypeOf(a) == Subtype_Yes) {
    return a;
  }

  return *Ty_Void;
}

} // namespace

long s_id_number;

// ctor
TypeVar::TypeVar() :
    id_number_(static_cast<int>(::InterlockedIncrement(&s_id_number))),
    type_(nullptr),
    types_(3) {
  types_.Add(Ty_Object);
}

// [A]
const Type& TypeVar::And(const Type& type) {
  Type::Set tyset;
  if (auto const tyvar = type.DynamicCast<TypeVar>()) {
    foreach (Type::List::Enum, as, types_) {
      auto& a = *as.Get();
      ASSERT(!a.Is<TypeVar>());
      foreach (Type::List::Enum, bs, tyvar->types_) {
        auto& b = *bs.Get();
        ASSERT(!b.Is<TypeVar>());
        auto& c = TypeAnd(a, b);
        if (c == *Ty_Object) {
          tyset.Add(&a);
          tyset.Add(&b);
        } else if (c != *Ty_Void) {
          tyset.Add(&c);
        }
      }
    }
  } else {
    foreach (Type::List::Enum, types, types_) {
      auto& a = *types.Get();
      ASSERT(!a.Is<TypeVar>());
      auto& b = type;
      ASSERT(!b.Is<TypeVar>());
      auto& c = TypeAnd(a, b);
        if (c == *Ty_Object) {
          tyset.Add(&a);
          tyset.Add(&b);
        } else if (c != *Ty_Void) {
          tyset.Add(&c);
        }
    }
  }

  types_.Clear();
  types_.AddAll(tyset);

  #ifdef _DEBUG
    if (!types_.Count()) {
      DEBUG_FORMAT("%s is now empty!", *this);
    }
  #endif

  return *this;
}

// [C]
void TypeVar::Close() {
  if (types_.Count() == 1) {
    type_ = types_.Get(0);
    ASSERT(!type_->Is<TypeVar>());
  }
}

const Type& TypeVar::ComputeBoundType() const {
  return type_ ? *type_ : *this;
}

int TypeVar::ComputeHashCode() const {
  return id_number_;
}

const Type& TypeVar::Construct(const TypeArgs& type_args) const {
  return type_ ? type_->Construct(type_args) : *this;
}

// [I]
bool TypeVar::IsBound() const {
  return type_ != nullptr;
}

bool TypeVar::IsEmpty() const {
  return types_.Count() == 0;
}

bool TypeVar::IsFixed() const {
  return types_.Count() <= 1;
}

Subtype TypeVar::IsSubtypeOf(const Type& r) const {
  if (*this == r || r == *Ty_Object) {
    return Subtype_Yes;
  }

  return Subtype_Unknown;
}

// [O]
const Type& TypeVar::Or(const Type& type) {
  if (types_.Count() == 1 && types_[0] == Ty_Object) {
    return *this;
  }

  if (type == *Ty_Object) {
    types_.Clear();
    types_.Add(Ty_Object);
  }

  if (auto const tyvar = type.DynamicCast<TypeVar>()) {
    foreach (Type::List::Enum, types, tyvar->types_) {
      auto& ty = *types.Get();
      ASSERT(!ty.Is<TypeVar>());
      Or(ty);
    }
    return *this;
  }

  Type::List cur_types(types_);
  types_.Clear();
  Type::Set tyset;
  foreach (Type::List::Enum, types, cur_types) {
    auto& curty = *types.Get();
    auto& orty = TypeOr(curty, type);
    if (orty == *Ty_Void) {
      if (!tyset.Contains(&type)) {
        tyset.Add(&type);
        types_.Add(&type);
      }

      if (!tyset.Contains(&curty)) {
        tyset.Add(&curty);
        types_.Add(&curty);
      }
    
    } else if (!tyset.Contains(&orty)) {
      tyset.Add(&orty);
      types_.Add(&orty);
    }
  }
  return *this;
}

// [T]
String TypeVar::ToString() const {
  if (IsBound()) {
    return String::Format("$T.%d=%s", id_number_, type_);
  }

  return String::Format("$T.%d(%s)", id_number_, types_);
}

} // Ir
} // Il
