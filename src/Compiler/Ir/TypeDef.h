// @(#)$Id$
//
// Evita Compiler - IR TypeDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_TypeDef_h)
#define INCLUDE_Compiler_Ir_TypeDef_h

#include "./ClassOrNamespaceDef.h"

namespace Compiler {

using Il::Ir::WithModifiers;

class NamespaceDef;

/// <summary>
///   Class TypeDef is base class for type representation in compiler.
/// </summary>
class TypeDef :
    public NameDef_<TypeDef, ClassOrNamespaceDef>,
    public WithModifiers {
  CASTABLE_CLASS(TypeDef);

  // ctor
  protected: TypeDef(
      ClassOrNamespaceDef&,
      int,
      const Name&,
      const SourceInfo&);

  DISALLOW_COPY_AND_ASSIGN(TypeDef);
};

template<typename T, typename B = TypeDef>
class TypeDef_ :
    public WithCastable4_<
        T,
        B,
        ClassOrNamespaceDef&,
        int,
        const Name&,
        const SourceInfo&> {

  protected: typedef TypeDef_<T, B> Base;

  protected: TypeDef_(
      ClassOrNamespaceDef& owner,
      int const modifiers,
      const Name& name,
      const SourceInfo& source_info)
      : WithCastable4_(owner, modifiers, name, source_info) {
  }

  public: virtual void Apply(Functor* const pFunctor) override {
    pFunctor->Process(static_cast<T*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(TypeDef_);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_TypeDef_h)
