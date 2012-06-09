#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Type
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ValuesTypeBuilder.h"

#include "./ValuesType.h"

namespace Il {
namespace Ir {

// ctor
ValuesTypeBuilder::ValuesTypeBuilder(int const capacity) :
    type_list_(capacity) {}

ValuesTypeBuilder::ValuesTypeBuilder(const Type::Collection& types) {
  foreach (Type::Collection::Enum, en, types) {
    Append(*en.Get());
  }
}

ValuesTypeBuilder::ValuesTypeBuilder(const ValuesType& values_type) {
  foreach (ValuesType::Enum, types, values_type) {
    Append(*types);
  } // for type
}

// [A]
void ValuesTypeBuilder::Append(const Type& ty) {
  ASSERT(!ty.Is<ValuesType>());
  type_list_.Add(&ty);
}

// [C]
void ValuesTypeBuilder::Clear() {
  type_list_.Clear();
} // Clear

int ValuesTypeBuilder::Count() const {
  return type_list_.Count();
} // Count

// [G]
const Type& ValuesTypeBuilder::Get(int const index) const {
  return *type_list_.Get(index);
} // Get

const ValuesType& ValuesTypeBuilder::GetValuesType() const {
  return ValuesType::Intern(ValuesType::Types(type_list_));
} // Get

// [S]
void ValuesTypeBuilder::Set(int const index, const Type& ty) {
  ASSERT(!ty.Is<ValuesType>());
  type_list_.Set(index, &ty);
} // Set

} // Ir
} // Il
