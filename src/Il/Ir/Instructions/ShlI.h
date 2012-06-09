// @(#)$Id$
//
// Evita Il - IR - Instructions - ShlI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ShlI_h)
#define INCLUDE_Il_Ir_Instructions_ShlI_h

#include "./ArithmeticInstruction.h"

namespace Il {
namespace Ir {

class ShlI :
      public Instruction_<ShlI, Op_Shl, ArithmeticInstruction> {

  // ctor
  public: ShlI();

  public: ShlI(const Type&, const Register&, const Operand&, const Operand&);
  public: ShlI(const Type*, const Register*, const Operand*, const Operand*);

  DISALLOW_COPY_AND_ASSIGN(ShlI);
}; // ShlI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ShlI_h)
