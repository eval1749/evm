#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- TypeDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeDef.h"

#include "./ClassDef.h"
#include "./NamespaceDef.h"

#include "../../Common/GlobalMemoryZone.h"

#include "../../Il/Ir/Operands/Namespace.h"

namespace Compiler {

// ctor
TypeDef::TypeDef(
    ClassOrNamespaceDef& owner,
    int const iModifiers,
    const Name& name,
    const SourceInfo& source_info)
    : Base(owner, name, source_info),
      WithModifiers(iModifiers) {}

} // Compiler
