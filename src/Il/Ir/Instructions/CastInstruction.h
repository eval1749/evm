// @(#)$Id$
//
// Evita Il - IR - Instructions - CastInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_CastInstruction_h)
#define INCLUDE_Il_Ir_Instructions_CastInstruction_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class CastInstruction : public InstructionFamily_<CastInstruction> {
  CASTABLE_CLASS(CastInstruction)

  // ctor
  protected: CastInstruction(
      Op const eOp,
      const Type* const pType,
      const Output* pOutput)
      : Base(eOp, pType, pOutput) {}

  DISALLOW_COPY_AND_ASSIGN(CastInstruction);
}; // CastInstruction

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_CastInstruction_h)
