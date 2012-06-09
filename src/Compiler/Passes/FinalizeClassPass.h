// @(#)$Id$
//
// Evita Compiler - FinalizeClassPass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_FinalizeClassPass_h)
#define INCLUDE_Compiler_Passes_FinalizeClassPass_h

#include "./AbstractResolvePass.h"

namespace Compiler {

// This pass resolves type of fields.
class FinalizeClassPass :
    public CompilePass_<FinalizeClassPass, AbstractResolvePass> {

  // ctor
  public: FinalizeClassPass(CompileSession*);

  // [P]
  public: virtual void Process(ClassDef*) override;

  DISALLOW_COPY_AND_ASSIGN(FinalizeClassPass);
}; // FinalizeClassPass

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_FinalizeClassPass_h)
