// @(#)$Id$
//
// Evita Il - IR - Instructions - CatchI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_CatchI_h)
#define INCLUDE_Il_Ir_Instructions_CatchI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class CatchI : public Instruction_<CatchI, Op_Catch> {
  // ctor
  public: CatchI();
  public: CatchI(const FrameReg&, const Operand&, const BBlock&);
  public: CatchI(const FrameReg*, const Operand*, const BBlock*);

  // [I]
  public: virtual bool IsUseless() const override { return false; }

  DISALLOW_COPY_AND_ASSIGN(CatchI);
}; // CatchI


} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_CatchI_h)
