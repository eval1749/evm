// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Tasks_NameRefResolver_h)
#define INCLUDE_Compiler_Tasks_NameRefResolver_h

#include "../../Il/Tasks/Tasklet.h"

namespace Compiler {

using Il::Tasks::Tasklet;

class PredefinedMethod;

class NameRefResolver : public Tasklet {
  private: CompileSession& compile_session_;
  private: const MethodDef& method_def_;

  // ctor
  public: NameRefResolver(CompileSession&, const MethodDef&);

  // [F]
  private: void FixLastNameRef(
      NameRefI&,
      const Operand&,
      const NameRef::Item&,
      const Operand&);
  private: const Operand* FixMiddleNameRef(
      const NameRefI&,
      const Operand&,
      const NameRef::Item&,
      const Operand&);

  // [L]
  private: const Operand* LookupMostOuterName(
      const NameRef&,
      const Class&,
      const NamespaceBody&);

  // [R]
  public: void Resolve(NameRefI&);
  private: const Operand* ResolveNameItem(
    const Operand&,
    const NameRef&,
    const NameRef::Item&);
  private: const Operand* ResolveNameItemForClass(
    const Class&,
    const NameRef::Item&);
  private: const Operand* ResolveNameItemForTypeParam(
    const TypeParam&,
    const NameRef::Item&);

  // [S]
  private: const Operand& SelectBaseOperand(
    CompileError error_operand_is_an_instance,
    CompileError error_operand_is_a_static,
    const NameRef::Item& name_item,
    const Operand& base_operand,
    const Operand& this_operand,
    bool is_static);

  DISALLOW_COPY_AND_ASSIGN(NameRefResolver);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Tasks_NameRefResolver_h)
