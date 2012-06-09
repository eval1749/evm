// @(#)$Id$
//
// Evita Il - IR - Instructions - ShrI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ShrI_h)
#define INCLUDE_Il_Ir_Instructions_ShrI_h

#include "./ArithmeticInstruction.h"

namespace Il {
namespace Ir {

class ShrI :
        public Instruction_<ShrI, Op_Shl, ArithmeticInstruction> {
    // ctor
    public: ShrI();

    public: ShrI(
        Type* const pty,
        Register* const pRd,
        Operand* const pSx,
        Operand* const pSy);

    DISALLOW_COPY_AND_ASSIGN(ShrI);
}; // ShrI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ShrI_h)
