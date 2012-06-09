// @(#)$Id$
//
// Evita Compiler - FieldDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_FieldDef_h)
#define INCLUDE_evc_FieldDef_h

#include "./NameDef.h"

namespace Compiler {

class FieldDef : public NameDef_<FieldDef> {
  CASTABLE_CLASS(FieldDef)

  public: typedef DoubleLinkedList_<FieldDef, ClassDef> List;

  private: int const modifiers_;
  private: const Type& type_;

  // ctor
  public: FieldDef(
      ClassDef&,
      int,
      const Type&,
      const Name&,
      const SourceInfo&);

  // [G]
  public: int GetModifiers() const { return modifiers_; }
  public: const Type* GetTy() const { return &type_; }

  // [R]
  public: void Realize(Field& field) { RealizeInternal(field); }

  DISALLOW_COPY_AND_ASSIGN(FieldDef);
}; // FieldDef

template<typename T, typename B = FieldDef>
class FieldDef_
    : public WithCastable5_<T, B, ClassDef&, int, const Type&, const Name&,
                            const SourceInfo&> {

  protected: typedef FieldDef_<T, B> Base;

  protected: FieldDef_(
      ClassDef& owner,
      int const modifiers,
      const Type& type,
      const Name& name,
      const SourceInfo& source_info)
      : WithCastable5_(owner, modifiers, type, name, source_info) {}

  public: virtual void Apply(Functor* const functor) override {
    ASSERT(functor != nullptr);
    functor->Process(static_cast<T*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(FieldDef_);
}; // FieldDef_

} // Compiler

#endif // !defined(INCLUDE_evc_FieldDef_h)
