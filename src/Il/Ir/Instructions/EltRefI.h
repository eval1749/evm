// @(#)$Id$
//
// Evita Il - IR - Instructions - EltRefI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_EltRefI_h)
#define INCLUDE_Il_Ir_Instructions_EltRefI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class EltRefI : public Instruction_<EltRefI, Op_EltRef> {
  public: EltRefI();

  public: EltRefI(
      const PointerType&,
      const Register&,
      const Operand&,
      const Operand&);

  DISALLOW_COPY_AND_ASSIGN(EltRefI);
}; // EltRefI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_EltRefI_h)
