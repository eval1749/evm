// @(#)$Id$
//
// Evita Il - IR - Instructions - MvRestoreI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_MvRestoreI_h)
#define INCLUDE_Il_Ir_Instructions_MvRestoreI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// MvRestoreI ty %vd <= %rx
class MvRestoreI
    : public Instruction_<MvRestoreI, Op_MvRestore> {

  // ctor
  public: MvRestoreI();
  public: MvRestoreI(Values* pVd, Register* pRx);
  public: MvRestoreI(Values* pVd, PseudoOutput* pRx);

    DISALLOW_COPY_AND_ASSIGN(MvRestoreI);
}; // MvRestoreI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_MvRestoreI_h)
