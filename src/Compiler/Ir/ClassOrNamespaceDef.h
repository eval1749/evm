// @(#)$Id$
//
// Evita Compiler - ClassOrNamespaceDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_NameDictionay_h)
#define INCLUDE_Compiler_Ir_NameDictionay_h

#include "../../Common/Collections/HashMap_.h"
#include "../../Common/Collections/PtrKey_.h"

#include "./NameDef.h"

namespace Compiler {

using Common::Collections::HashMap_;

/// <summary>
///   Class ClassOrNamespaceDef is abstract base class of ClassDef and
///   NamespaceDef.
/// </summary>
class ClassOrNamespaceDef : public NameDef_<ClassOrNamespaceDef, NameDef> {
  CASTABLE_CLASS(ClassOrNamespaceDef);

  private: typedef HashMap_<const Name*, NameDef*> NameTable;

  // name_defs_ contains list of name declarations in source code order.
  private: NameDef::List name_defs_;

  // name_table_ is used for looking name up.
  private: NameTable name_table_;

  // ctor
  protected: ClassOrNamespaceDef(
      NameDef&,
      const Name& name,
      const SourceInfo&);

  public: virtual ~ClassOrNamespaceDef() {}

  public: const NameDef::List& members() const { return name_defs_; }

  // [A]
  public: void Add(NameDef&);

  // [F]
  public: NameDef* Find(const Name&) const;

  DISALLOW_COPY_AND_ASSIGN(ClassOrNamespaceDef);
}; // ClassOrNamespaceDef

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_NameDictionay_h)
