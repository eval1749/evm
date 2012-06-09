// @(#)$Id$
//
// Evita Compiler - TypeNameResolver.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Compiler_Syntax_TypeNameResolver_h)
#define INCLUDE_Evita_Compiler_Syntax_TypeNameResolver_h

#include "./TokenProcessor.h"
#include "../../Common/Collections/HashSet_.h"

namespace Compiler {

interface TypeNameResolver {
  virtual const Type& ResolveTypeName(NameRef*) = 0;
};

} // Compiler

#endif // !defined(INCLUDE_Evita_Compiler_Syntax_TypeNameResolver_h)
