// @(#)$Id$
//
// Evita Il - IR - Instructions - VarDefI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_VarDefI_h)
#define INCLUDE_Il_Ir_Instructions_VarDefI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// VARDEF ty %rd <= var %sy
class VarDefI : public Instruction_<VarDefI, Op_VarDef> {
  // ctor
  public: VarDefI();

  public: VarDefI(
    const Class&,
    const Register&,
    const Variable&,
    const Operand&);

  // [I]
  public: virtual bool IsUseless() const override;

  DISALLOW_COPY_AND_ASSIGN(VarDefI);
}; // VarDefI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_VarDefI_h)
