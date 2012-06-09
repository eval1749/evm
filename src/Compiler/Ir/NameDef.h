// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_NameDef_h)
#define INCLUDE_Compiler_Ir_NameDef_h

#include "./NamespaceMember.h"

namespace Compiler {

class ClassDef;
class ClassOrNamespaceDef;
class NamespaceDef;
class ResolvePass;
class Functor;

class NameDef
    : public DoubleLinkedItem_<NameDef, ClassOrNamespaceDef>,
      public Entity_<NameDef, NamespaceMember> {
  CASTABLE_CLASS(NameDef);

  public: typedef DoubleLinkedList_<NameDef, ClassOrNamespaceDef> List;

  private: const Name& name_;
  private: NameDef& owner_;
  private: Operand* operand_;

  // ctor
  protected: NameDef(
      NameDef& owner,
      const Name& name,
      const SourceInfo&);

  public: const Name& name() const { return name_; }
  public: Operand& operand() const;
  public: NameDef& owner() const { return owner_; }


  // [C]
  public: int ComputeHashCode() const;

  // [I]
  public: bool IsGlobalNamespaceDef() const;
  public: bool IsRealized() const { return operand_ != nullptr; }

  // [R]
  protected: void RealizeInternal(Operand&);

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(NameDef);
}; // NameDef

inline int ComputeHashCode(const NameDef* const p) {
  ASSERT(p != nullptr);
  return p->ComputeHashCode();
}

template<typename T, typename B = NameDef>
class NameDef_
    : public WithCastable3_<T, B, NameDef&, const Name&, const SourceInfo&> {

  private: typedef WithCastable3_<T, B, NameDef&, const Name&,
      const SourceInfo&> TempalteBase;

  protected: typedef NameDef_<T, B> Base;

  protected: NameDef_(
      NameDef& owner,
      const Name& name,
      const SourceInfo& source_info)
      : TempalteBase(owner, name, source_info) {}

  public: virtual void Apply(Functor* const functor) override {
    ASSERT(functor != nullptr);
    functor->Process(static_cast<T*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(NameDef_);
}; // NameDef_

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_NameDef_h)
