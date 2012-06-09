// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_TypeVarRewriter_h)
#define INCLUDE_Compiler_Passes_TypeVarRewriter_h

#include "../../Il/Tasks/Tasklet.h"

#include "../../Il/Tasks/NormalizeTasklet.h"

namespace Compiler {

using Il::Tasks::NormalizeTasklet;
using Il::Tasks::Tasklet;

class PredefinedMethod;

class TypeVarRewriter : public Tasklet {
  private: CompileSession& compile_session_;
  private: NormalizeTasklet normalizer_;

  // ctor
  public: TypeVarRewriter(CompileSession&, const Module&);

  // [C]
  private: Field& ComputeBoundField(const Field&);
  private: Method& ComputeBoundMethod(const CallI&, const Method&);
  private: Method& ComputeBoundMethod(const CallI&);
  private: const Type& ComputeBoundType(const Type&);

  // [F]
  public: void Finish();
  private: bool FixInsturction(Instruction&);
  private: void FixOperatorCall(CallI&);
  private: bool FixOutputType(Instruction&, const Type&);

  // [R]
  public: void Rewrite(Instruction&);
  private: void RewriteOperator(CallI&, const PredefinedMethod&);

  DISALLOW_COPY_AND_ASSIGN(TypeVarRewriter);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_TypeVarRewriter_h)
