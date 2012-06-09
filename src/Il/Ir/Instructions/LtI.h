// @(#)$Id$
//
// Evita Il - IR - Instructions - LtI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_LtI_h)
#define INCLUDE_Il_Ir_Instructions_LtI_h

#include "./RelationalInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   LT instruction
/// </summary>
/// <example>
///     LT bool %bd &lt;= %sx %sy
/// </example>
class LtI : public Instruction_<LtI, Op_Lt, RelationalInstruction> {
  // ctor
  public: LtI();

  public: LtI(
    const BoolOutput&,
    const Operand&,
    const Operand&);

  public: LtI(
    const BoolOutput*,
    const Operand*,
    const Operand*);

  DISALLOW_COPY_AND_ASSIGN(LtI);
}; // LtI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_LtI_h)
