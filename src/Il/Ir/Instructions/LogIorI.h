// @(#)$Id$
//
// Evita Il - IR - Instructions - LogIorI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_LogIorI_h)
#define INCLUDE_Il_Ir_Instructions_LogIorI_h

#include "./ArithmeticInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   ADD instruction.
///   <para>
///     ADD ty %rd &lt;= %sx %sy
///   </para>
///   <list>
///     <item>
///       <term>%rd</term>
///       <description>
///         A GPR contains result of ADD operation.
///       </description>
///     </item>
///     <item>
///       <term>%sx</term>
///       <description>An operand</description>
///     </item>
///     <item>
///       <term>%sy</term>
///       <description>An operand</description>
///     </item>
///  </list>
/// </summary>
class LogIorI :
    public Instruction_<LogIorI, Op_LogIor, ArithmeticInstruction> {

    // ctor
    public: LogIorI();

    public: LogIorI(
        Type* const pty,
        Register* const pRd,
        Operand* const pSx,
        Operand* const pSy );

    DISALLOW_COPY_AND_ASSIGN(LogIorI);
}; // LogIorI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_LogIorI_h)
