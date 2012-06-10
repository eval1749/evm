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
    public: EnumConstraint(const TypeParamDef* p)
        : Base(&const_cast<TypeParamDef*>(p)->m_oConstraints) {}
    DISALLOW_COPY_AND_ASSIGN(EnumConstraint);
  };

  private: bool m_fNewable;
  private: Type::List m_oConstraints;
  private: TypeParam& type_param_;

  // ctor
  public: TypeParamDef(ClassOrNamespaceDef&, const Name&, const SourceInfo&);

  // properties
  public: bool is_newable() const { return m_fNewable; }
  public: const TypeParam& type_param() const { return type_param_; }

  // [A]
  public: void Add(const Type&);

  DISALLOW_COPY_AND_ASSIGN(TypeParamDef);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_TypeParamDef_h)
