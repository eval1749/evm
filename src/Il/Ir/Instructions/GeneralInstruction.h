// @(#)$Id$
//
// Evita Il - IR - Instructions - GeneralInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_GeneralInstruction_h)
#define INCLUDE_Il_Ir_Instructions_GeneralInstruction_h

#include "./Instruction.h"

namespace Il {
namespace Ir {

class GeneralInstruction :
    public InstructionFamily_<GeneralInstruction, Instruction> {

    CASTABLE_CLASS(GeneralInstruction)

    // ctor
    protected: GeneralInstruction(
        Op const eOp,
        const Type* const pType,
        const Output* pOutput) :
            Base(eOp, pType, pOutput) {}

    DISALLOW_COPY_AND_ASSIGN(GeneralInstruction);
}; // GeneralInstruction

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_GeneralInstruction_h)
