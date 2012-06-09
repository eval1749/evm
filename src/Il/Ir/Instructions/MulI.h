// @(#)$Id$
//
// Evita Il - IR - Instructions - MulI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_MulI_h)
#define INCLUDE_Il_Ir_Instructions_MulI_h

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
class MulI : public Instruction_<MulI, Op_Mul, ArithmeticInstruction> {
  public: MulI();

  public: MulI(
    const Type&,
    const Register&,
    const Operand&,
    const Operand&);

  public: MulI(
    const Type*,
    const Register*,
    const Operand*,
    const Operand*);

  DISALLOW_COPY_AND_ASSIGN(MulI);
}; // MulI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_MulI_h)
