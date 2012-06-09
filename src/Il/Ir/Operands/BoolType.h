// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_BoolType_h)
#define INCLUDE_Il_Ir_BoolType_h

#include "./Type.h"

namespace Il {
namespace Ir {

class BoolType : public Type_<BoolType> {
  CASTABLE_CLASS(BoolType);

  // ctor
  public: BoolType();

  // [C]
  public: virtual int ComputeHashCode() const override;
  public: virtual Type& Construct(const TypeArgs&) const override;

  // [I]
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(BoolType);
}; // BoolType

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_BoolType_h)
