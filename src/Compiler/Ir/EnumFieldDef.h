// @(#)$Id$
//
// Evita Compiler - EnumFieldDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_EnumFieldDef_h)
#define INCLUDE_evc_EnumFieldDef_h

#include "./FieldDef.h"

namespace Compiler {

class EnumFieldDef : public FieldDef_<EnumFieldDef> {
  CASTABLE_CLASS(EnumFieldDef)

  private: const Operand* expr_;

  // ctor
  public: EnumFieldDef(
      ClassDef&,
      int,
      const Type&,
      const Name&,
      const SourceInfo&);

  // properties
  public: const Operand& expr() const;
  public: void set_expr(const Operand&);

  DISALLOW_COPY_AND_ASSIGN(EnumFieldDef);
}; // EnumFieldDef

} // Compiler

#endif // !defined(INCLUDE_evc_EnumFieldDef_h)
