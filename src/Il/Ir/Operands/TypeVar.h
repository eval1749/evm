// @(#)$Id$
//
// Evita IL - IR - ValuesType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_TypeVar_h)
#define INCLUDE_Il_Ir_TypeVar_h

#include "./Type.h"

namespace Il {
namespace Ir {

/// <summary>
///     Parse only?
/// </summary>
class TypeVar : public Type_<TypeVar> {
  CASTABLE_CLASS(TypeVar)

  public: class Enum : public Type::List::Enum {
    private: typedef Type::List::Enum Base;
    public: Enum(const TypeVar& r) : Base(r.types_) {}
    DISALLOW_COPY_AND_ASSIGN(Enum);
  };

  private: int const id_number_;
  private: const Type* type_;
  private: Type::List types_;

  // ctor
  public: TypeVar();
  public: virtual ~TypeVar() {}

  // [A]
  public: const Type& And(const Type&);

  // [C]
  public: void Close();
  public: virtual const Type& ComputeBoundType() const override;
  public: virtual int ComputeHashCode() const override;
  public: virtual const Type& Construct(const TypeArgs&) const override;

  // [I]
  public: virtual bool IsBound() const override;
  public: bool IsEmpty() const;
  public: bool IsFixed() const;
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [O]
  public: const Type& Or(const Type&);

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(TypeVar);
}; // TypeVar

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_TypeVar_h)
