// @(#)$Id$
//
// Evita Il - IR - Instructions - ThrowI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_ThrowI_h)
#define INCLUDE_Il_Ir_Instructions_ThrowI_h

#include "./TerminateInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   THROW instruction.
///   <para>
///     THROW %sx
///   </para>
///   <list>
///     <item>
///       <term>%sx</term>
///       <description>
///         A value of containing function.
///       </description>
///     </item>
///   </list>
/// </summary>
class ThrowI :
        public Instruction_<ThrowI, Op_Throw, TerminateInstruction> {

    // ctor
    public: ThrowI();
    public: ThrowI(Operand* const pSx);

    DISALLOW_COPY_AND_ASSIGN(ThrowI);
}; // ThrowI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_ThrowI_h)
