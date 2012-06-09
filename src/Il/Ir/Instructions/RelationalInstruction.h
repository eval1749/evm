// @(#)$Id$
//
// Evita Il - IR - Instructions - RelationalInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_RelationalInstruction_h)
#define INCLUDE_Il_Ir_Instructions_RelationalInstruction_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class RelationalInstruction
    : public InstructionFamily_<RelationalInstruction> {

  CASTABLE_CLASS(RelationalInstruction)

  // ctor
  protected: RelationalInstruction(
      Op const opcode,
      const Type& type,
      const Output& output)
      : Base(opcode, type, output) {}

  protected: RelationalInstruction(
      Op const opcode,
      const Type* const type,
      const Output* output)
      : Base(opcode, type, output) {}

  DISALLOW_COPY_AND_ASSIGN(RelationalInstruction);
}; // RelationalInstruction

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_RelationalInstruction_h)
