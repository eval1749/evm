// @(#)$Id$
//
// Evita Il - IR - Instructions - StoreI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_StoreI_h)
#define INCLUDE_Il_Ir_Instructions_StoreI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// STORE %rx %sy
class StoreI : public Instruction_<StoreI, Op_Store> {
  // ctor
  public: StoreI();
  public: StoreI(const Operand&, const Operand&);
  DISALLOW_COPY_AND_ASSIGN(StoreI);
}; // StoreI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_StoreI_h)
