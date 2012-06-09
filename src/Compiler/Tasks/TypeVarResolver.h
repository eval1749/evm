// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Tasks_TypeVarResolver_h)
#define INCLUDE_Compiler_Tasks_TypeVarResolver_h

#include "../../Il/Tasks/Tasklet.h"

#include "../../Il/Tasks/NormalizeTasklet.h"

namespace Compiler {

using Il::Tasks::NormalizeTasklet;
using Il::Tasks::Tasklet;

class PredefinedMethod;

class TypeVarResolver : public Tasklet {
  private: CompileSession& compile_session_;
  private: const MethodDef& method_def_;
  private: ArrayList_<Instruction*> type_var_insts_;
  private: HashSet_<const TypeVar*> type_var_set_;

  // ctor
  public: TypeVarResolver(CompileSession&, const MethodDef&);

  // [A]
  public: void Add(Instruction&);

  // [F]
  private: void FixTypeVarWithMethodCall(const CallI&);

  // [R]
  public: void Resolve();

  // [U]
  private: bool UpdateTypeVar(const Instruction&);
  private: bool UpdateTypeVarWithCall(const CallI&);
  private: bool UpdateTypeVarWithMethod(const CallI&, const Method&);
  private: bool UpdateTypeVarWithMethodGroup(const CallI&);
  private: bool UpdateTypeVarWithOperator(const CallI&);
  private: bool UnifyTypes(const Instruction&, const Type&, const Type&);

  DISALLOW_COPY_AND_ASSIGN(TypeVarResolver);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Tasks_TypeVarResolver_h)
