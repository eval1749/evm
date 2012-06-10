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

  private: const Name& name_;

  // Member varible owner_ contains Class or Method object.
  private: const Operand* owner_;

  // ctor
  public: explicit TypeParam(const Name&);

  // properties
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

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(TypeParam);
}; // TypeParam

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_TypeParam_h)
