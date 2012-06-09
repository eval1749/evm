#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- MethodParam
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MethodParamDef.h"

#include "./VariableOwner.h"

namespace Compiler {

// ctor
MethodParamDef::MethodParamDef(
    VariableOwner& owner,
    const Type& type,
    const Name& name,
    const Variable& variable,
    const SourceInfo& source_info)
    : Base(owner, type, name, variable, source_info),
      m_iIndex(owner.CountParams()) {}

} // Compiler
