// @(#)$Id$
//
// Evita Il - IR - PrimitiveType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_PrimitiveType_h)
#define INCLUDE_Il_Ir_PrimitiveType_h

#include "./Class.h"

namespace Il {
namespace Ir {

class PrimitiveType : public Class_<PrimitiveType> {
  CASTABLE_CLASS(PrimitiveType);

  private: bool const has_sign_;
  private: RegClass const reg_class_;
  private: int const bit_width_;

  // ctor
  public: PrimitiveType(
      const Name& name,
      RegClass const eRegClass,
      int const bit_width,
      bool const fHasSign);

  public: virtual int bit_width() const override {
    return bit_width_;
  }

  // [G]
  public: virtual RegClass GetRegClass() const override { 
    return reg_class_;
  }

  public: virtual bool HasSign() const override {
    return has_sign_;
  }

  DISALLOW_COPY_AND_ASSIGN(PrimitiveType);
}; // PrimitiveType

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_PrimitiveType_h)
