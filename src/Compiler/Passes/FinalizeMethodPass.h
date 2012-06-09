// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_FinalizeMethodPass_h)
#define INCLUDE_Compiler_Passes_FinalizeMethodPass_h

#include "./AbstractResolvePass.h"
#include "../../Il/Tasks/NormalizeTasklet.h"

namespace Compiler {

using namespace Il::Tasks;

class PredefinedMethod;

class FinalizeMethodPass
    : public CompilePass_<FinalizeMethodPass, AbstractResolvePass> {
  // ctor
  public: FinalizeMethodPass(CompileSession*);

  // [F]
  private: void FixFunction(MethodDef&, Function&);

  // [P]
  private: virtual void Process(MethodDef* const pMethodDef) override;

  DISALLOW_COPY_AND_ASSIGN(FinalizeMethodPass);
}; // FinalizeMethodPass

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_FinalizeMethodPass_h)
