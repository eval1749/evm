#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- EnumFieldDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./EnumFieldDef.h"

#include "./ClassDef.h"

namespace Compiler {

// ctor
EnumFieldDef::EnumFieldDef(
    ClassDef& owner,
    int const modifiers,
    const Type& type,
    const Name& name,
    const SourceInfo& source_info)
    :  Base(owner, modifiers, type, name, source_info),
       expr_(nullptr) {}

// properties
const Operand& EnumFieldDef::expr() const {
  ASSERT(!!expr_);
  return *expr_;
}

void EnumFieldDef::set_expr(const Operand& expr) {
  ASSERT(!expr_);
  expr_ = &expr;
}

} // Compiler
