// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_NewI_h)
#define INCLUDE_Il_Ir_Instructions_NewI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// NewI ty %rd <=
class NewI : public Instruction_<NewI, Op_New> {
  // ctor
  public: NewI();
  public: NewI(const Type&, const Register&);
  DISALLOW_COPY_AND_ASSIGN(NewI);
}; // NewI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_NewI_h)
