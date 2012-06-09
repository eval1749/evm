// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_FinalizeEnumPass_h)
#define INCLUDE_Compiler_Passes_FinalizeEnumPass_h

#include "./AbstractResolvePass.h"

#include "../../Common/Collections.h"

namespace Compiler {

// FinalzeEnumPass computes actual value of enum fields which defined
// using expression.
//
// Enum field expressions are represented as IR instructions stored in
// class constructor.
class FinalizeEnumPass
    : public CompilePass_<FinalizeEnumPass, AbstractResolvePass> {

  private: Queue_<Class*> pending_queue_;

  // ctor
  public: FinalizeEnumPass(CompileSession* const pCompileSession);

  // [F]
  bool FixStaticConstFields(Class* const pClass);

  // [P]
  public: virtual void Process(ClassDef* const pClassDef) override;

  // [S]
  public: virtual void Start() override;

  DISALLOW_COPY_AND_ASSIGN(FinalizeEnumPass);
}; // FinalizeEnumPass

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_FinalizeEnumPass_h)
