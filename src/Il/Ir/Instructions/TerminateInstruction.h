// @(#)$Id$
//
// Evita Il - IR - Instructions - TerminateInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_TerminateInstruction_h)
#define INCLUDE_Il_Ir_Instructions_TerminateInstruction_h

#include "./LastInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   Family members of Terminate instruction:
///   <list>
///     <item>Ret</item>
///     <item>Unreachable</item>
///   </list>
/// </summary>
class TerminateInstruction :
        public InstructionFamily_<TerminateInstruction, LastInstruction> {

    CASTABLE_CLASS(TerminateInstruction)

    // ctor
    protected: TerminateInstruction(
        Op const eOp,
        const Type* const pType,
        const Output* pOutput) :
            Base(eOp, pType, pOutput) {}

    // [R]
    public: virtual void Realize() override;

    // [U]
    public: virtual void Unrealize() override;

    DISALLOW_COPY_AND_ASSIGN(TerminateInstruction);
}; // TerminateInstruction

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_TerminateInstruction_h)
