// @(#)$Id$
//
// Evita Il - IR - Instructions - CloseI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_CloseI_h)
#define INCLUDE_Il_Ir_Instructions_CloseI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class CloseI : public Instruction_<CloseI, Op_Close> {
  // ctor
  public: CloseI();
  public: CloseI(FrameReg* const pRx);

  // [I]
  public: virtual bool IsUseless() const override;

  DISALLOW_COPY_AND_ASSIGN(CloseI);
}; // CloseI


} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_CloseI_h)
