// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_ConstructedClass_h)
#define INCLUDE_Il_Ir_ConstructedClass_h

#include "./Class.h"

#include "./TypeArgs.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

// This class represent open and closed constructed class. On open constructed
// class some type parameters are unbound.
class ConstructedClass
    : public WithCastable1_<
        ConstructedClass,
        Class,
        const GenericClass&> {
  CASTABLE_CLASS(ConstructedClass);

  public: class EnumTypeArg : public TypeArgs::Enum {
    private: typedef TypeArgs::Enum Base;
    public: EnumTypeArg(const ConstructedClass& r) : Base(r.type_args_) {}
  };

  private: const GenericClass& generic_class_;
  private: const TypeArgs type_args_;

  friend class GenericClass;

  // ctor
  private: ConstructedClass(const GenericClass&, const TypeArgs&);

  // properties
  public: const GenericClass& generic_class() const { return generic_class_; }
  public: const TypeArgs& type_args() const { return type_args_; }

  // [A]
  public: virtual void Apply(Functor* const functor) override {
    functor->Process(this);
  }

  // [C]
  public: virtual Type& ComputeBoundType() const override;
  public: virtual Type& Construct(const TypeArgs&) const override;
  public: Method& ConstructMethod(MethodGroup&, const Method&) const;

  // [G]
  public: const Type& GetTypeArg() const;
  public: const Type* GetTypeArgOf(const TypeParam&) const;

  // [I]
  public: virtual bool IsBound() const override;
  public: bool IsOpen() const;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(ConstructedClass);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_ConstructedClass_h)
