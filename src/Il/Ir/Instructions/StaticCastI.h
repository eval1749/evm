// @(#)$Id$
//
// Evita Il - IR - Instructions - StaticCastI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_StaticCastI_h)
#define INCLUDE_Il_Ir_Instructions_StaticCastI_h

#include "./CastInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   STATICCAST instruction.
///   <para>
///     STATICCAST ty %rd &lt;= %rx
///   </para>
///   <list>
///     <item>
///       <term>%rd</term>
///       <description>
///         A GPR contains result of STATICCAST operation.
///       </description>
///     </item>
///     <item>
///       <term>%rx</term>
///       <description>An operand</description>
///     </item>
///  </list>
/// </summary>
class StaticCastI
    : public Instruction_<StaticCastI, Op_StaticCast, CastInstruction> {
  public: StaticCastI();
  public: StaticCastI(const Type&, const SsaOutput&, const Operand&);
  DISALLOW_COPY_AND_ASSIGN(StaticCastI);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_StaticCastI_h)
