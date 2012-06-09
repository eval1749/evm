#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- VarDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./VarDef.h"

#include "./MethodDef.h"

namespace Compiler {

// ctor
VarDef::VarDef(
    VariableOwner& method_def,
    const Type& type,
    const Name& name,
    const Variable& variable,
    const SourceInfo& source_info) 
    : Base(method_def, name, source_info),
      var_type_(&type),
      variable_(variable) {
  ASSERT(type != *Ty_Void);
  const_cast<Variable&>(variable_).SetTy(type);
}

// [S]
void VarDef::SetTy(const Type& type) {
  var_type_ = &type;
}

} // Compiler
