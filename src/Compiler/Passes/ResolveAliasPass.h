// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_ResolveAliasPass_h)
#define INCLUDE_Compiler_Passes_ResolveAliasPass_h

#include "./AbstractResolvePass.h"

namespace Compiler {

class ResolveAliasPass
    : public CompilePass_<ResolveAliasPass, AbstractResolvePass> {

  // ctor
  public: ResolveAliasPass(CompileSession* const pCompileSession);

  // [P]
  public: override void Process(AliasDef* const pAliasDef);

  // [S]
  public: override void Start();

  DISALLOW_COPY_AND_ASSIGN(ResolveAliasPass);
}; // ResolveAliasPass

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_ResolveAliasPass_h)
