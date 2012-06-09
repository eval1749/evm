// @(#)$Id$
//
// Evita Il - IR - Instructions - NeI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_NeI_h)
#define INCLUDE_Il_Ir_Instructions_NeI_h

#include "./RelationalInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   LT instruction
/// </summary>
/// <example>
///     LT bool %bd &lt;= %sx %sy
/// </example>
class NeI : public Instruction_<NeI, Op_Ne, RelationalInstruction> {
  // ctor
  public: NeI();

  public: NeI(
      const BoolOutput&,
      const Operand&,
      const Operand&);

  public: NeI(
      const BoolOutput*,
      const Operand*,
      const Operand*);

  DISALLOW_COPY_AND_ASSIGN(NeI);
}; // NeI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_NeI_h)
