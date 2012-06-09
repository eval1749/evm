// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_ArrayType_h)
#define INCLUDE_Il_Ir_ArrayType_h

#include "./Type.h"

#include "../../../Common/Collections/HashMap_.h"

namespace Il {
namespace Ir {

using Common::Collections::HashMap_;

class ArrayType : public Type_<ArrayType> {
  CASTABLE_CLASS(ArrayType);

  private: int const rank_;
  private: const Type& element_type_;

  private: ArrayType(const Type&, int);
  public: virtual ~ArrayType() {}

  public: const Type& element_type() const { return element_type_; }
  public: int rank() const { return rank_; }

  // [C]
  public: virtual const Type& ComputeBoundType() const override;
  public: virtual int ComputeHashCode() const override;
  public: virtual const Type& Construct(const TypeArgs&) const override;

  // [I]
  public: static const ArrayType& LibExport Intern(const Type&, int = 1);

  public: virtual bool IsBound() const override;
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [M]
  public: static void LibExport MapAll(Functor&);

  // [T]
  public: virtual String ToString() const override;

  // [U]
  public: void Unintern();

  DISALLOW_COPY_AND_ASSIGN(ArrayType);
}; // ArrayType

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_ArrayType_h)
