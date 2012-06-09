#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Type
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Type.h"

#include "./Class.h"
#include "./PrimitiveType.h"
#include "./ValuesType.h"
#include "./ValuesTypeBuilder.h"

namespace Il {
namespace Ir {

// ctor
Type::Type() {}

Type::~Type() {
  DEBUG_PRINTF("%p\n", this);
}

// [I]
bool Type::IsFloat() const {
  return GetRegClass() == RegClass_Fpr;
}

bool Type::IsInt() const {
 return GetRegClass() == RegClass_Gpr && bit_width() > 0;
}

bool Type::IsUInt() const {
return IsInt() && !HasSign();
}

// [O]
const Type& Type::Or(const Type& type_a, const Type& type_b) {
  if (type_a == *Ty_Void) {
    return type_b;
  }

  if (type_b == *Ty_Void) {
    return type_a;
  }

  if (auto const values_type_a = type_a.DynamicCast<ValuesType>()) {
    if (auto const values_type_b = type_b.DynamicCast<ValuesType>()) {
      if (values_type_a->Count() == values_type_b->Count()) {
        ValuesTypeBuilder builder(values_type_a->Count());
        ValuesType::Enum enum_b(*values_type_b);
        foreach (ValuesType::Enum, enum_a, values_type_a) {
          builder.Append(Type::Or(*enum_a, *enum_b));
          enum_b.Next();
        }
        return builder.GetValuesType();
      }
    }
    return *Ty_Void;
  }

  if (type_b.Is<ValuesType>()) {
    return *Ty_Void;
  }

  if (type_a.IsSubtypeOf(type_b) == Subtype_Yes) {
    return type_b;
  }

  if (type_b.IsSubtypeOf(type_a) == Subtype_Yes) {
    return type_a;
  }

  return *Ty_Void;
}

} // Ir
} // Il
