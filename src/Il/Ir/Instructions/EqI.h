// @(#)$Id$
//
// Evita Il - IR - Instructions - EqI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_EqI_h)
#define INCLUDE_Il_Ir_Instructions_EqI_h

#include "./RelationalInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   LT instruction
/// </summary>
/// <example>
///     LT bool %bd &lt;= %sx %sy
/// </example>
class EqI : public Instruction_<EqI, Op_Eq, RelationalInstruction> {
  public: EqI();
  public: EqI(const BoolOutput&, const Operand&, const Operand&);

  // For Compiler DEFOPERATOR_COMP
  public: EqI(const BoolOutput*, const Operand*, const Operand*);

  DISALLOW_COPY_AND_ASSIGN(EqI);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_EqI_h)
