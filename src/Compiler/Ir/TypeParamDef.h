// @(#)$Id$
//
// Evita Compiler - IR TypeDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_TypeParamDef_h)
#define INCLUDE_Compiler_Ir_TypeParamDef_h

#include "./TypeDef.h"

namespace Compiler {

class TypeParamDef : public TypeDef_<TypeParamDef> {
  CASTABLE_CLASS(TypeParamDef);

  public: class EnumConstraint : public Type::List::Enum {
    private: typedef Type::List::Enum Base;
    public: EnumConstraint(const TypeParamDef&r )
        : Base(&r.constraints_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumConstraint);
  };

  private: Type::List constraints_;
  private: int newable_;
  private: const NameDef* owner_;
  private: TypeParam& type_param_;

  // ctor
  public: TypeParamDef(ClassOrNamespaceDef&, const Name&, const SourceInfo&);

  // properties
  public: bool is_newable() const { return !!newable_; }
  public: const NameDef& owner() const { ASSERT(!!owner_); return *owner_; }
  public: const TypeParam& type_param() const { return type_param_; }
  public: void set_owner(const NameDef&);

  // [A]
  public: void AddConstraint(const Type&);

  // [M]
  public: void MarkNewable();

  DISALLOW_COPY_AND_ASSIGN(TypeParamDef);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_TypeParamDef_h)
