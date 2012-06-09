// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_VarDef_h)
#define INCLUDE_evc_VarDef_h

#include "./NameDef.h"

namespace Compiler {

class VarDef
    : public DoubleLinkedItem_<VarDef, VariableOwner>,
      public NameDef_<VarDef> {
  CASTABLE_CLASS(VarDef);

  public: typedef DoubleLinkedList_<VarDef, VariableOwner> List;

  private: const Variable& variable_;
  private: const Type* var_type_;

  // ctor
  protected: VarDef(
      VariableOwner&,
      const Type&,
      const Name&,
      const Variable&,
      const SourceInfo&);

  // properties
  public: const Type& type() const { return *var_type_; }
  public: void set_type(const Type& type) { var_type_ = &type; }

  // [G]
  public: const Type* GetTy() const { return var_type_; }
  public: const Variable& GetVariable() const { return variable_; }

  // [S]
  public: void SetTy(const Type&);

  DISALLOW_COPY_AND_ASSIGN(VarDef);
}; // VarDef

template<typename T, typename B = VarDef>
class VarDef_ :
  public WithCastable5_<T, B, VariableOwner&, const Type&, const Name&,
                        const Variable&, const SourceInfo&> {

  protected: typedef VarDef_<T, B> Base;

  protected: VarDef_(
      VariableOwner& method_def,
      const Type& type,
      const Name& name,
      const Variable& variable,
      const SourceInfo& source_info)
      : WithCastable5_(
              method_def,
              type,
              name,
              variable,
              source_info) {}

  DISALLOW_COPY_AND_ASSIGN(VarDef_);
}; // VarDef_

} // Compiler

#endif // !defined(INCLUDE_evc_VarDef_h)
