// @(#)$Id$
//
// Evita Il - IR - Instructions - GtI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_GtI_h)
#define INCLUDE_Il_Ir_Instructions_GtI_h

#include "./RelationalInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   LT instruction
/// </summary>
/// <example>
///     LT bool %bd &lt;= %sx %sy
/// </example>
class GtI : public Instruction_<GtI, Op_Gt, RelationalInstruction> {
  // ctor
  public: GtI();

  public: GtI(
    const BoolOutput&,
    const Operand&,
    const Operand&);

  public: GtI(
    const BoolOutput*,
    const Operand*,
    const Operand*);

  DISALLOW_COPY_AND_ASSIGN(GtI);
}; // GtI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_GtI_h)
