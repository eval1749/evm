// @(#)$Id$
//
// Evita Compiler - FinalizeUsingNamespacePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_FinalizeUsingNamespacePass_h)
#define INCLUDE_Compiler_Passes_FinalizeUsingNamespacePass_h

#include "./AbstractResolvePass.h"

namespace Compiler {

// This class realizes
//  o using alias declaration for namespace alias.
//  o using directive
//
// ResolveClassPass will realize class alias.
class FinalizeUsingNamespacePass
    : public CompilePass_<FinalizeUsingNamespacePass, AbstractResolvePass> {

  // ctor
  public: FinalizeUsingNamespacePass(CompileSession*);

  // [P]
  private: virtual void Process(AliasDef*) override;
  private: virtual void Process(UsingNamespace*) override;

  DISALLOW_COPY_AND_ASSIGN(FinalizeUsingNamespacePass);
}; // FinalizeUsingNamespacePass

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_FinalizeUsingNamespacePass_h)
