// @(#)$Id$
//
// Evita Compiler - VariableOwner
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Compiler_Syntax_VariableOwner_h)
#define INCLUDE_evm_Compiler_Syntax_VariableOwner_h

#include "./ClassOrNamespaceDef.h"

#include "./MethodParamDef.h"

namespace Compiler {

class VariableOwner : public NameDef_<VariableOwner, ClassOrNamespaceDef> {
  public: class EnumParam : public MethodParamDef::List::Enum {
    private: typedef MethodParamDef::List::Enum Base;
    public: EnumParam(const VariableOwner& r) :Base(&r.method_params_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumParam);
  };

  private: MethodParamDef::List method_params_;

  // ctor
  protected: VariableOwner(NameDef&, const Name&, const SourceInfo&);

  // [A]
  protected: void AddParam(MethodParamDef&);

  // [C]
  // For MethodParamDef.index_
  public: int CountParams() const { return method_params_.Count(); }

  DISALLOW_COPY_AND_ASSIGN(VariableOwner);
};

} // Compiler

#endif // !defined(INCLUDE_evm_Compiler_Syntax_VariableOwner_h)
