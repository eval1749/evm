// @(#)$Id$
//
// Evita Il - IR - Instructions - ReinterpretCastI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ReinterpretCastI_h)
#define INCLUDE_Il_Ir_Instructions_ReinterpretCastI_h

#include "../../Il/Ir/Instructions/CastInstruction.h"

namespace Il {
namespace Cg {

// ReinterpretCastI ty %rd <= %rx
class ReinterpretCastI : public CgInstruction_<ReinterpretCastI,
                                               Op_ReinterpretCast> {
  // ctor
  public: ReinterpretCastI() : Base(Ty_Void, Void) {}

  public: ReinterpretCastI(
      const Type& outy,
      const Register& out,
      const Operand& src)
      : Base(outy, out) {
    AppendOperand(src);
  }

  // [I]
  public: virtual bool IsUseless() const override {
    return output() == op0();
  }

  DISALLOW_COPY_AND_ASSIGN(ReinterpretCastI);
}; // ReinterpretCastI

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ReinterpretCastI_h)
