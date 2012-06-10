// @(#)$Id$
//
// Evita Il - IR - TypeParam
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_TypeParam_h)
#define INCLUDE_Il_Ir_TypeParam_h

#include "./Class.h"

namespace Il {
namespace Ir {

/// <summary>
///     This class represents type parameter. Type parameter is created
///     without owner, since generic method and generic function signatures
//      contain type parameters bound to owner, e.g. T Foo<T>(T x).
/// </summary>
class TypeParam : public Type_<TypeParam> {
  CASTABLE_CLASS(TypeParam)

  public: enum ConstructorConstraint {
    NotNewable,
    Newable,
  };

  public: typedef Collection_<const Class*> Constraints;

  public: class EnumConstraint : public Constraints::Enum {
    private: typedef Constraints::Enum Base;
    public: EnumConstraint(const TypeParam& r) : Base(r.class_constraints_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumConstraint);
  };

  // Member varible owner_ contains Class or Method object.
  private: Constraints class_constraints_;
  private: ConstructorConstraint ctor_constraint_;
  private: const Name& name_;
  private: const Operand* owner_;
  private: bool realized_;

  // ctor
  public: explicit TypeParam(const Name&);

  // properties
  public: bool is_newable() const;
  public: bool is_realized() const { return realized_; }
  public: const Name& name() const { return name_; }
  public: const Operand& owner() const;

  // [B]
  public: void BindTo(const Operand&);

  // [C]
  public: virtual int ComputeHashCode() const override;
  public: virtual const Type& Construct(const TypeArgs&) const override;

  // [I]
  public: bool IsBound() const { return !!owner_; }
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [R]
  public: void RealizeTypeParam(
      const Constraints&,
      ConstructorConstraint);

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(TypeParam);
}; // TypeParam

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_TypeParam_h)
