#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- UnresolvedConstructType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./UnresolvedConstructedType.h"

#include "./NameRef.h"

namespace Il {
namespace Ir {

// ctor
UnresolvedConstructedType::UnresolvedConstructedType(
    const NameRef& name_ref,
    const Type::Collection& types)
    : Base(name_ref),
      types_(types) {}

// [C]
int UnresolvedConstructedType::ComputeHashCode() const {
  auto hash_code = Common::ComputeHashCode(
      name_ref().ComputeHashCode(), 'u');
  foreach (Type::Collection::Enum, en, types_) {
    auto const ty = en.Get();
    hash_code = Common::ComputeHashCode(ty->ComputeHashCode(), hash_code);
  }
  return hash_code;
}

// [T]
String UnresolvedConstructedType::ToString() const {
  StringBuilder builder;
  builder.AppendFormat("?%s<", name_ref());
  const char* comma = "";
  foreach (Type::Collection::Enum, it, types_) {
    builder.Append(comma);
    comma = ", ";
    builder.Append(it.Get()->ToString());
  }
  builder.Append(">");
  if (IsBound()) {
    builder.AppendFormat("=%s", resolved_type());
  }
  return builder.ToString();
}

} // Ir
} // Il
