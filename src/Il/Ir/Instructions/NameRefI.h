// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_NameRefI_h)
#define INCLUDE_Il_Ir_Instructions_NameRefI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

// NameRefI ty %rd <= %rx
class NameRefI : public Instruction_<NameRefI, Op_NameRef> {
  public: NameRefI();

  public: NameRefI(
      const Type&,
      const Register&,
      const Operand&,
      const NameRef&);

  DISALLOW_COPY_AND_ASSIGN(NameRefI);
}; // NameRefI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_NameRefI_h)
