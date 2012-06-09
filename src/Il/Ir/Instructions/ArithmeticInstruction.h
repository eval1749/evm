// @(#)$Id$
//
// Evita Il - IR - Instructions - ArithmeticInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ArithmeticInstruction_h)
#define INCLUDE_Il_Ir_Instructions_ArithmeticInstruction_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class ArithmeticInstruction :
    public InstructionFamily_<ArithmeticInstruction> {

    CASTABLE_CLASS(ArithmeticInstruction)

    // ctor
    protected: ArithmeticInstruction(
        Op const eOp,
        const Type* const pType,
        const Output* pOutput) :
            Base(eOp, pType, pOutput) {}

    DISALLOW_COPY_AND_ASSIGN(ArithmeticInstruction);
}; // ArithmeticInstruction

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ArithmeticInstruction_h)
