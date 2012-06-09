// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_MethodDef_h)
#define INCLUDE_Compiler_Ir_MethodDef_h

#include "./VariableOwner.h"

#include "./MethodParamDef.h"
#include "./VarDef.h"

namespace Compiler {

using Il::Ir::Module;
using Il::Ir::Function;
using Il::Ir::WithModifiers;

// MethodDef provides name dictionary for type parametername.
// MethodDef should be ClassOrNamespaceDef for owner of TypeParamDef and provide
// type parameter names for name resolution in type parameter constraints
// processing.
class MethodDef
    : public DoubleLinkedItem_<MethodDef, MethodGroupDef>,
      public NameDef_<MethodDef, VariableOwner>,
      public WithModifiers {

  CASTABLE_CLASS(MethodDef)

  public: typedef DoubleLinkedList_<MethodDef, MethodGroupDef> List;

  public: class EnumTypeParam : public ArrayList_<TypeParamDef*>::Enum {
    private: typedef ArrayList_<TypeParamDef*>::Enum Base;
    public: EnumTypeParam(const MethodDef& r) : Base(&r.type_params_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumTypeParam);
  };

  public: class EnumVar : public VarDef::List::Enum {
    private: typedef VarDef::List::Enum Base;
    public: EnumVar(const MethodDef& r) : Base(&r.vars_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumVar);
  };

  private: Module& module_;
  private: Function* function_;
  private: int const index_;
  private: MethodGroupDef& method_group_;
  private: NamespaceBody& namespace_body_;

  // return_type_ will be update to Type Param in MethodDef::Add.
  private: const Type* return_type_;
  private: const NameRef& qualified_name_;

  private: ArrayList_<TypeParamDef*> type_params_;

  // this_operand_ contains Register for instance method or Void for static
  // method.
  private: Operand& this_operand_;
  private: VarDef::List vars_;

  // ctor
  public: MethodDef(
      NamespaceBody&,
      MethodGroupDef&,
      int const,
      const Type&,
      const NameRef&);

  public: virtual ~MethodDef();

  public: Function* function() const { return function_; }
  public: int index() const { return index_; }
  public: Method& method() const;
  public: Module& module() const { return module_; }
  public: NamespaceBody& namespace_body() const { return namespace_body_; }
  public: Operand& this_operand() const { return this_operand_; }
  public: MethodGroupDef& method_group_def() const { return method_group_; }
  public: ClassDef& owner_class_def() const;
  public: const Type& return_type() const { return *return_type_; }
  public: const NameRef& qualified_name() const { return qualified_name_; }
  public: Collection_<TypeParamDef*> type_raram_defs() const;

  public: void set_function(Function&);

  // [A]
  public: void Add(MethodParamDef&);
  public: void Add(TypeParamDef&);
  public: void Add(VarDef&);

  // [R]
  public: void Realize(Method&);

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(MethodDef);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_MethodDef_h)
