// @(#)$Id$
//
// Evita Compiler - Method Resolver
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_MethodResolver_h)
#define INCLUDE_Compiler_MethodResolver_h

#include "../../Il/Tasks/Tasklet.h"

namespace Compiler {

using namespace Il::Tasks;

class MethodResolver : public Tasklet {
  private: CompileSession& m_compile_session;
  private: const Module& module_;
  private: const SourceInfo source_info_;

  // ctor
  public: MethodResolver(CompileSession&, const Module&, const SourceInfo&);

  // [B]
  private: bool BindTypeArg(TypeArgs&, const Type&, const Type&) const;
  private: const Operand& BoxingIfNeeded(OperandBox&, const Type&);

  // [C]
  public: ArrayList_<Method*> ComputeApplicableMethods(const CallI&) const;
  private: ArrayList_<Method*> ComputeApplicableMethods(
      const CallI&,
      const MethodGroup&) const;

  // [F]
  public: void FixMethodArgs(CallI&);

  // [I]
  public: void static Init();
  private: bool IsApplicable(TypeArgs&, const Method&, const CallI&) const;
  private: bool IsApplicable(const Method&, const CallI&) const;

  // [U]
  private: bool UnifyTypes(TypeArgs&, const Type&, const Type&) const;

  DISALLOW_COPY_AND_ASSIGN(MethodResolver);
}; // MethodResolver

} // Compiler

#endif // !defined(INCLUDE_Compiler_MethodResolver_h)
