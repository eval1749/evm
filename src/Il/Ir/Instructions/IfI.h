// @(#)$Id$
//
// Evita Il - IR - Instructions - IfI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_IfI_h)
#define INCLUDE_Il_Ir_Instructions_IfI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class IfI : public Instruction_<IfI, Op_If> {
  // ctor
  public: IfI();

  public: IfI(
    const Type&,
    const Output&,
    const BoolOutput&,
    const Operand&,
    const Operand&);

  // [I]
  public: static bool IsNot(const Instruction&);

  DISALLOW_COPY_AND_ASSIGN(IfI);
}; // IfI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_IfI_h)
