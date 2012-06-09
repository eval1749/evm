#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- LocalLocalVarDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LocalVarDef.h"

#include "./MethodDef.h"

namespace Compiler {

LocalVarDef::LocalVarDef(
    MethodDef& method_def,
    const Type& type,
    const Name& name,
    const Variable& variable,
    const SourceInfo& source_info)
        : Base(method_def, type, name, variable, source_info) {}

} // Compiler
