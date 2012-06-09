// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_MethodGroupDef_h)
#define INCLUDE_evc_MethodGroupDef_h

#include "./NameDef.h"

#include "./MethodDef.h"

namespace Compiler {

class MethodGroupDef : public NameDef_<MethodGroupDef> {
  CASTABLE_CLASS(MethodGroupDef)

  public: class EnumMethodDef : public MethodDef::List::Enum {
    public: EnumMethodDef(const MethodGroupDef& r)
        : MethodDef::List::Enum(&r.method_defs_) {}
  };

  private: MethodDef::List method_defs_;

  // ctor
  public: MethodGroupDef(ClassDef&, const Name&);

  // properties
  public: MethodGroup& method_group() const;
  public: ClassDef& owner_class_def() const;

  // [A]
  public: void Add(MethodDef&);

  // [C]
  // For MethodDef.GetIndex
  public: int CountMethods() const;

  // [R]
  public: void Realize(MethodGroup& mtg) { RealizeInternal(mtg); }

  DISALLOW_COPY_AND_ASSIGN(MethodGroupDef);
};

} // Compiler

#endif // !defined(INCLUDE_evc_MethodGroupDef_h)
