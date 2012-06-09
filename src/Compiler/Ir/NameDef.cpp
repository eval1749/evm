#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- NameDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NameDef.h"

#include "./NamespaceDef.h"

#include "../Ir.h"

namespace Compiler {

NameDef::NameDef(
    NameDef& owner,
    const Name& name,
    const SourceInfo& source_info)
    : Base(&source_info),
      name_(name),
      operand_(nullptr),
      owner_(owner) {}

Operand& NameDef::operand() const {
  ASSERT(operand_ != nullptr);
  return *operand_;
}

// [C]
int NameDef::ComputeHashCode() const {
  return name_.ComputeHashCode();
}

// [I]
bool NameDef::IsGlobalNamespaceDef() const {
  return this == &owner_ ;
}

// [R]
void NameDef::RealizeInternal(Operand& operand) {
  DEBUG_FORMAT("%s %s %s", GetKind(), this, operand);
  ASSERT(!operand_);
  operand_ = &operand;
}

// [T]
String NameDef::ToString() const {
  return name().ToString();
}
} // Compiler
