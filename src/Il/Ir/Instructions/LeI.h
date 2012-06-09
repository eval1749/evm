// @(#)$Id$
//
// Evita Il - IR - Instructions - LeI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_LeI_h)
#define INCLUDE_Il_Ir_Instructions_LeI_h

#include "./RelationalInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   LT instruction
/// </summary>
/// <example>
///     LT bool %bd &lt;= %sx %sy
/// </example>
class LeI : public Instruction_<LeI, Op_Le, RelationalInstruction> {
  // ctor
  public: LeI();

  public: LeI(
    const BoolOutput&,
    const Operand&,
    const Operand&);

  public: LeI(
    const BoolOutput*,
    const Operand*,
    const Operand*);

  DISALLOW_COPY_AND_ASSIGN(LeI);
}; // LeI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_LeI_h)
