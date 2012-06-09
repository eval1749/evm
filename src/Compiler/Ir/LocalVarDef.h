// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_LocalVarDef_h)
#define INCLUDE_Compiler_Ir_LocalVarDef_h

#include "./VarDef.h"

namespace Compiler {

class LocalVarDef
    : public DoubleLinkedItem_<LocalVarDef, MethodDef>,
      public VarDef_<LocalVarDef> {
  CASTABLE_CLASS(LocalVarDef);
  public: typedef DoubleLinkedList_<LocalVarDef, MethodDef> List;

  // ctor
  public: LocalVarDef(
      MethodDef&,
      const Type&,
      const Name& name,
      const Variable&,
      const SourceInfo&);

  DISALLOW_COPY_AND_ASSIGN(LocalVarDef);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_LocalVarDef_h)
