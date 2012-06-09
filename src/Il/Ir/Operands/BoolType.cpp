#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Type
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./BoolType.h"

namespace Il {
namespace Ir {

static bool s_fInitialized;

// ctor
BoolType::BoolType() {
    ASSERT(!s_fInitialized);
    s_fInitialized = true;
}

// [C]
int BoolType::ComputeHashCode() const { return 1; }

Type& BoolType::Construct(const TypeArgs&) const {
  return *const_cast<BoolType*>(this);
}

// [I]
Subtype BoolType::IsSubtypeOf(const Type& r) const {
  return *this == r ? Subtype_Yes : Subtype_No;
}

// [T]
String BoolType::ToString() const { return "BoolType"; }

} // Ir
} // Il
