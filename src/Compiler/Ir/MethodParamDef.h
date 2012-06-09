// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_MethodParamDef_h)
#define INCLUDE_Compiler_Ir_MethodParamDef_h

#include "./VarDef.h"

namespace Compiler {

class MethodParamDef
    : public DoubleLinkedItem_<MethodParamDef, VariableOwner>,
      public VarDef_<MethodParamDef> {
  CASTABLE_CLASS(MethodParamDef)

  public: typedef DoubleLinkedList_<MethodParamDef, VariableOwner> List;

  private: int const m_iIndex;

  // ctor
  public: MethodParamDef(
      VariableOwner&,
      const Type&,
      const Name& name,
      const Variable&,
      const SourceInfo&);

  // [G]
  // TODO 2011-01-17 yosi@msn.com Do we really need to have
  // MethodParamDef.GetIndex?
  private: int GetIndex() const { return m_iIndex; }

  DISALLOW_COPY_AND_ASSIGN(MethodParamDef);
}; // MethodParamDef

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_MethodParamDef_h)
