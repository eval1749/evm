// @(#)$Id$
//
// Evita Il - IR - Instructions - LogXorI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_LogXorI_h)
#define INCLUDE_Il_Ir_Instructions_LogXorI_h

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
class LogXorI
    : public Instruction_<LogXorI, Op_LogXor, ArithmeticInstruction> {

  public: LogXorI();

  public: LogXorI(
    const Type&,
    const Register&,
    const Operand&,
    const Operand&);

  public: LogXorI(
    const Type*,
    const Register*,
    const Operand*,
    const Operand*);

    DISALLOW_COPY_AND_ASSIGN(LogXorI);
}; // LogXorI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_LogXorI_h)
