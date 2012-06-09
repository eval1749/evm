// @(#)$Id$
//
// Evita Compiler
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

  private: NameRef* const name_ref_;

  // ctor
  public: AliasDef(NamespaceDef*, Name*, NameRef*, const SourceInfo*);

  // properties
  public: NameRef* name_ref() const { return name_ref_; }

  DISALLOW_COPY_AND_ASSIGN(AliasDef);
}; // AliasDef

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_AliasDef_h)
