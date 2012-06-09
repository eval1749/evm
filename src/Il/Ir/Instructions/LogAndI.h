// @(#)$Id$
//
// Evita Il - IR - Instructions - LogAndI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_LogAndI_h)
#define INCLUDE_Il_Ir_Instructions_LogAndI_h

#include "./ArithmeticInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   LOGAND instruction.
///   <para>
///     LOGAND ty %rd &lt;= %sx %sy
///   </para>
///   <list>
///     <item>
///       <term>%rd</term>
///       <description>
///         A GPR contains result of LOGAND operation.
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
class LogAndI :
    public Instruction_<LogAndI, Op_LogAnd, ArithmeticInstruction> {
    // ctor
    public: LogAndI();

    public: LogAndI(
        Type* const pty,
        Register* const pRd,
        Operand* const pSx,
        Operand* const pSy );

    DISALLOW_COPY_AND_ASSIGN(LogAndI);
}; // LogAndI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_LogAndI_h)
