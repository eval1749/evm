// @(#)$Id$
//
// Evita Il - IR - Instructions - OpenExitPointInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_OpenExitPointInstruction_h)
#define INCLUDE_Il_Ir_Instructions_OpenExitPointInstruction_h

#include "./OpenInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   Family members of Terminate instruction:
///   <list>
///     <item>Ret</item>
///     <item>Unreachable</item>
///   </list>
/// </summary>
class OpenExitPointInstruction :
        public InstructionFamily_<OpenExitPointInstruction, OpenInstruction> {

    CASTABLE_CLASS(OpenExitPointInstruction)

    // ctor
    protected: OpenExitPointInstruction(
        Op const eOp,
        const Type* const pType,
        const Output* const pOutput) :
            Base(eOp, pType, pOutput) {}

    DISALLOW_COPY_AND_ASSIGN(OpenExitPointInstruction);
}; // OpenExitPointInstruction

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_OpenExitPointInstruction_h)
