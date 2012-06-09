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

  private: static long s_lIndex;

  // TODO 2011-01-10 Encode new constraints into modifiers since type
  // parameter doesn't use modifier.
  private: bool const m_fNewable;
  private: int const m_iIndex;
  private: const Name& name_;
  private: Operand* m_pOwner;

  // ctor
  public: explicit TypeParam(const Name&);

  public: const Name& name() const { return name_; }

  // [B]
  public: void BindTo(Operand* const pOwner);

  // [C]
  public: virtual int ComputeHashCode() const override;
  public: virtual const Type& Construct(const TypeArgs&) const override;

  // [G]
  public: Operand* GetOwner() const;
  public: GenericClass* GetOwnerClass() const;
  public: GenericMethod* GetOwnerMethod() const;

  // [I]
  public: bool IsBound() const { return m_pOwner != nullptr; }
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(TypeParam);
}; // TypeParam

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_TypeParam_h)
