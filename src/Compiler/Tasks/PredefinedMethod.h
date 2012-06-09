// @(#)$Id$
//
// Evita Compiler - Method Resolver
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_PredefinedMethod_h)
#define INCLUDE_Compiler_PredefinedMethod_h

#include "../../Il/Ir/Operands/Method.h"

namespace Compiler {

using namespace Il::Ir;

class PredefinedMethod : public Method_<PredefinedMethod, Method> {
  CASTABLE_CLASS(PredefinedMethod);

  public: enum RewriteRule {
    LhsCmpRhs,
    LhsEqFalse,
    LhsOpMinusOne,
    LhsOpOne,
    LhsOpRhs,
    ZeroOpLhs,
  };

  private: Op opcode_;
  private: RewriteRule rewrite_rule_;

  public: PredefinedMethod(
      const MethodGroup&,
      int,
      const FunctionType&,
      Op,
      RewriteRule);

  public: Op opcode() const { return opcode_; }
  public: RewriteRule rewrite_rule() const { return rewrite_rule_; }

  DISALLOW_COPY_AND_ASSIGN(PredefinedMethod);
}; // PredefinedMethod

} // Compiler

#endif // !defined(INCLUDE_Compiler_PredefinedMethod_h)
