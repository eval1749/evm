// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_NullI_h)
#define INCLUDE_Il_Ir_Instructions_NullI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// NullI ty %rd <=
class NullI : public Instruction_<NullI, Op_Null> {
  public: NullI();
  public: NullI(const Type&, const Register&);
  DISALLOW_COPY_AND_ASSIGN(NullI);
}; // NullI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_NullI_h)
