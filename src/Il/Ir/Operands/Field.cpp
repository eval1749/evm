#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Field
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Field.h"

#include "../Name.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
Field::Field(
    Class& owner_class,
    int const modifiers,
    const Type& storage_type,
    const Name& name,
    const SourceInfo* const source_info)
    : WithSourceInfo(source_info),
      WithModifiers(modifiers),
      name_(name),
      offset_(-1),
      operand_(Void),
      owner_class_(owner_class),
      storage_type_(storage_type) {}

const Type& Field::field_type() const {
  return owner_class().GetBaseClass() == Ty_Enum
      ? owner_class()
      : storage_type_;
}

// [G]
// Note: Return enum type for cast instruction optimization.
const Type& Field::GetTy() const {
  ASSERT(owner_class().IsSubtypeOf(*Ty_Enum) == Subtype_Yes);
  return owner_class();
}

// [S]
void Field::SetOffset(int const offset) {
  ASSERT(offset >= 0);
  ASSERT(offset_ == -1);
  offset_ = offset;
} // SetOffset

void Field::SetOperand(const Operand* const operand) {
  ASSERT(operand != nullptr);
  operand_ = operand;
} // SetOperand

// [T]
String Field::ToString() const {
  return String::Format("%s.%s", owner_class_, name_);
}

} // Ir
} // Il
