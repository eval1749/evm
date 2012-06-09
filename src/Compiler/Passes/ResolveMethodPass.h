// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_ResolveMethodPass_h)
#define INCLUDE_Compiler_Passes_ResolveMethodPass_h

#include "./AbstractResolvePass.h"

namespace Compiler {

// ResolveMethodPass does
//  o install default constructor if needed
//  o install Method object to MethodDef
//      - Resolve return type and parameter types
//      - Update associated Function object.
class ResolveMethodPass
    : public CompilePass_<ResolveMethodPass, AbstractResolvePass> {

  // For Resolving type parameter in return value type.
  class ResolveContextForMethod : public ResolveContext {
    private: const MethodDef& method_def_;
    public: ResolveContextForMethod(const MethodDef&);
    public: virtual void Resolve(
        const Name&,
        ArrayList_<const Operand*>&) const;
    DISALLOW_COPY_AND_ASSIGN(ResolveContextForMethod);
  };

  // ctor
  public: ResolveMethodPass(CompileSession*);

  // [C]
  private: FunctionType& ComputeMethodSignature(const MethodDef&);

  // [I]
  private: MethodGroup& InternMethodGroup(MethodDef&);
  private: void InstallDefaultCtor(Class&, const SourceInfo&);

  // [P]
  public: virtual void Process(ClassDef*) override;
  public: virtual void Process(MethodDef*) override;

  DISALLOW_COPY_AND_ASSIGN(ResolveMethodPass);
}; // ResolveMethodPass

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_ResolveMethodPass_h)
