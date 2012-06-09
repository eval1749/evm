// @(#)$Id$
//
// Evita Il - Ir - PointerType.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_PointerType_h)
#define INCLUDE_Il_Ir_PointerType_h

#include "./Type.h"

namespace Il {
namespace Ir {

class PointerType : public Type_<PointerType> {
  CASTABLE_CLASS(PointerType);

  private: const Type& pointee_type_;

  // ctor
  private: PointerType(const Type&);
  public: virtual ~PointerType() {}

  public: const Type& pointee_type() const { return pointee_type_; }

  // [C]
  public: virtual const Type& ComputeBoundType() const override;
  public: virtual int ComputeHashCode() const override;
  public: virtual const Type& Construct(const TypeArgs&) const override;

  // [I]
  public: static const PointerType& LibExport Intern(const Type&);
  public: virtual bool IsBound() const override;
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [M]
  public: static void LibExport MapAll(Functor&);

  // [T]
  public: virtual String ToString() const override;

  // [U]
  public: void Unintern();

  DISALLOW_COPY_AND_ASSIGN(PointerType);
}; // PointerType

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_PointerType_h)
