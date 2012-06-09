// @(#)$Id$
//
// Evita Il - IR - Instructions - ClosureI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ClosureI_h)
#define INCLUDE_Il_Ir_Instructions_ClosureI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// ClosureI
class ClosureI : public Instruction_<ClosureI, Op_Closure> {
  // ctor
  public: ClosureI();

  public: ClosureI(
      FunctionType* const pFunty,
      Register* const pRd,
      Function* const pFun,
      Values*   const pVy);

  // Note: Since we need to propagate function type and notinline attribute
  // to CALL instruciton, we don't have Compute method for CLOSURE
  // instruction.

  DISALLOW_COPY_AND_ASSIGN(ClosureI);
}; // ClosureI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ClosureI_h)
