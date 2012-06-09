#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- VariableOwner
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./VariableOwner.h"

namespace Compiler {

// ctor
VariableOwner::VariableOwner(
    NameDef& owner,
    const Name& name,
    const SourceInfo& source_info)
    : Base(owner, name, source_info) {}

// [A]
void VariableOwner::AddParam(MethodParamDef& param) {
  ASSERT(param.owner() == *this);
  method_params_.Append(&param);
  Add(param);
}

} // Compiler
