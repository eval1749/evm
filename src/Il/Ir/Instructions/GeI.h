// @(#)$Id$
//
// Evita Il - IR - Instructions - GeI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_GeI_h)
#define INCLUDE_Il_Ir_Instructions_GeI_h

#include "./RelationalInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   LT instruction
/// </summary>
/// <example>
///     LT bool %bd &lt;= %sx %sy
/// </example>
class GeI : public Instruction_<GeI, Op_Ge, RelationalInstruction> {
  // ctor
  public: GeI();

  public: GeI(
    const BoolOutput&,
    const Operand&,
    const Operand&);

  public: GeI(
    const BoolOutput*,
    const Operand*,
    const Operand*);

  DISALLOW_COPY_AND_ASSIGN(GeI);
}; // GeI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_GeI_h)
