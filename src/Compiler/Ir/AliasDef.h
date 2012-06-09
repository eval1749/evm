// @(#)$Id$
//
// Evita Compiler Ir AliasDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_AliasDef_h)
#define INCLUDE_Compiler_Ir_AliasDef_h

#include "./NameDef.h"

namespace Compiler {

// Class AliasDef represent using alias directive.
// using-alias-directive ::=
//  'using' identifier '=' namespace-or-type-name ';'
class AliasDef : public NameDef_<AliasDef> {
  CASTABLE_CLASS(AliasDef);

  private: NamespaceBody* namespace_body_;
  private: const NameRef& real_name_;

  // ctor
  public: AliasDef(
      NamespaceBody*,
      const Name&,
      const NameRef&,
      const SourceInfo&);

  // properties
  public: NamespaceBody* namespace_body() const { return namespace_body_; }
  public: const NameRef* real_name() const { return &real_name_; }

  // [R]
  public: void Realize(Namespace& ns) { RealizeInternal(ns); }
  public: void Realize(Type& ty) { RealizeInternal(ty); }

  DISALLOW_COPY_AND_ASSIGN(AliasDef);
}; // AliasDef

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_AliasDef_h)
