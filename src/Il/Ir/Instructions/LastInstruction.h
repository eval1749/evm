// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_LastInstruction_h)
#define INCLUDE_Il_Ir_Instructions_LastInstruction_h

#include "./Instruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   Family members of Last instruction:
///   <list>
///     <item>Branch</item>
///     <item>Exit</item>
///     <item>Jump</item>
///   </list>
/// </summary>
class LastInstruction : public InstructionFamily_<LastInstruction> {
  CASTABLE_CLASS(LastInstruction)

  // ctor
  protected: LastInstruction(
      Op const eOp,
      const Type* const pType,
      const Output* pOutput);

  DISALLOW_COPY_AND_ASSIGN(LastInstruction);
}; // LastInstruction

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_LastInstruction_h)
