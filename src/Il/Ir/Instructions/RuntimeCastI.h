// @(#)$Id$
//
// Evita Il - IR - Instructions - RuntimeCastI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_RuntimeCastI_h)
#define INCLUDE_Il_Ir_Instructions_RuntimeCastI_h

#include "./CastInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   RUNTIMECAST instruction.
///   <para>
///     RUNTIMECAST ty %rd &lt;= %rx
///   </para>
///   <list>
///     <item>
///       <term>%rd</term>
///       <description>
///         A GPR contains result of RUNTIMECAST operation.
///       </description>
///     </item>
///     <item>
///       <term>%rx</term>
///       <description>An operand</description>
///     </item>
///  </list>
/// </summary>
class RuntimeCastI
    : public Instruction_<RuntimeCastI, Op_RuntimeCast, CastInstruction> {
  // ctor
  public: RuntimeCastI();
  public: RuntimeCastI(const Type&, const SsaOutput&, const Operand&);
  DISALLOW_COPY_AND_ASSIGN(RuntimeCastI);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_RuntimeCastI_h)
