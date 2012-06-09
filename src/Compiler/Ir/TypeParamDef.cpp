#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- TypeParamDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeParamDef.h"

namespace Compiler {

// ctor
TypeParamDef::TypeParamDef(
    ClassOrNamespaceDef& owner,
    const Name& name,
    const SourceInfo& source_info)
    : Base(owner, 0, name, source_info),
      m_fNewable(false),
      type_param_(*new TypeParam(name)) {}

// [A]
void TypeParamDef::Add(const Type& type) {
  m_oConstraints.Add(&type);
}

} // Compiler
