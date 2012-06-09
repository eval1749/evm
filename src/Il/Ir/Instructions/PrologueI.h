// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_PrologueI_h)
#define INCLUDE_Il_Ir_Instructions_PrologueI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class PrologueI : public Instruction_<PrologueI, Op_Prologue> {

  // ctor
  public: PrologueI();
  public: PrologueI(const Values&);

  // [I]
  public: virtual bool IsUseless() const override { return false; }

  DISALLOW_COPY_AND_ASSIGN(PrologueI);
}; // PrologueI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_PrologueI_h)
