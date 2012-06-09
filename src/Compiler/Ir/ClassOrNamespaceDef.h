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

  // Enumerates NameDef objects in ClassOrNamespaceDef in source code order.
  // Some unit tests depend on source code order for testing parser.
  public: class EnumMember : public NameDef::List::Enum {
    private: typedef NameDef::List::Enum Base;

    public: EnumMember(const ClassOrNamespaceDef& r)
        : Base(&r.name_defs_) {}

    public: NameDef* Get() const { return Base::Get(); }
  }; // EnumMember

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

  // [A]
  public: void Add(NameDef&);

  // [F]
  public: NameDef* Find(const Name&) const;

  DISALLOW_COPY_AND_ASSIGN(ClassOrNamespaceDef);
}; // ClassOrNamespaceDef

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_NameDictionay_h)
