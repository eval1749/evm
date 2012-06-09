// @(#)$Id$
//
// Evita Il - IR - Instructions - RetI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_RetI_h)
#define INCLUDE_Il_Ir_Instructions_RetI_h

#include "./TerminateInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   RET instruction.
///   <para>
///     RET %sx
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
class RetI :
        public Instruction_<RetI, Op_Ret, TerminateInstruction> {

    // ctor
    public: RetI();
    public: RetI(const Operand&);

    DISALLOW_COPY_AND_ASSIGN(RetI);
}; // RetI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_RetI_h)
