// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_NewArrayI_h)
#define INCLUDE_Il_Ir_Instructions_NewArrayI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// NewArrayI ty %rd <= %vx
class NewArrayI : public Instruction_<NewArrayI, Op_NewArray> {
  // ctor
  public: NewArrayI();
  public: NewArrayI(const ArrayType&, const Register&, const Values&);
  DISALLOW_COPY_AND_ASSIGN(NewArrayI);
}; // NewArrayI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_NewArrayI_h)
