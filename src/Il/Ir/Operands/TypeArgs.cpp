#include "precomp.h"
// @(#)$Id$
//
// Evita IL - IR- TypeArgs
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeArgs.h"

#include "./TypeParam.h"
#include "../../../Common/GlobalMemoryZone.h"

namespace Il {
namespace Ir {

// ctor
TypeArgs::TypeArgs(const TypeParam& p1, const Type& a1) {
  ASSERT(p1 != a1);
  binding_map_.Add(&p1, &a1);
}

TypeArgs::TypeArgs(const TypeArgs& r) {
  foreach (Enum, type_args, r) {
    binding_map_.Add(type_args.Get().GetKey(), type_args.Get().GetValue());
  }
}

TypeArgs::TypeArgs() {}

TypeArgs& TypeArgs::operator=(const TypeArgs& r) {
  binding_map_.Clear();
  foreach (Enum, type_args, r) {
    binding_map_.Add(type_args.Get().GetKey(), type_args.Get().GetValue());
  }
  return *this;
}

// [A]
void TypeArgs::Add(const TypeParam& typaram, const Type& type) {
  ASSERT(typaram != type);
  binding_map_.Add(&typaram, &type);
}

// [C]
void TypeArgs::Clear() {
  binding_map_.Clear();
}

int TypeArgs::ComputeHashCode() const {
  auto hash_code = binding_map_.Count();
  foreach (Enum, type_args, this) {
    hash_code = Common::ComputeHashCode(
        type_args.Get().GetValue()->ComputeHashCode(),
        hash_code);
  } // for
  return hash_code;
}

// [G]
const Type* TypeArgs::Get(const TypeParam& typaram) const {
  return binding_map_.Get(&typaram);
}

bool TypeArgs::HashEquals(const TypeArgs* const that) const {
  ASSERT(that != nullptr);

  if (this->Count() != that->Count()) {
    return false;
  }

  foreach (Enum, type_args, this) {
    auto& typaram = *type_args.Get().GetKey();
    if (that->Get(typaram) != type_args.Get().GetValue()) {
      return false;
    }
  }

  return true;
}

} // Ir
} // Il
