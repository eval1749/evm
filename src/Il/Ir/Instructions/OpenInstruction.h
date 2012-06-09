// @(#)$Id$
//
// Evita Il - IR - Instructions - OpenInstruction
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_OpenInstruction_h)
#define INCLUDE_Il_Ir_Instructions_OpenInstruction_h

#include "./Instruction.h"

namespace Il {
namespace Ir {

class OpenInstruction
    : public InstructionFamily_<OpenInstruction, Instruction> {

  CASTABLE_CLASS(OpenInstruction)

  // ctor
  protected: OpenInstruction(
      Op const eOp,
      const Type* const pType,
      const Output* const pOutput);

  // [I]
  protected: bool IsNextClose() const;
  public: virtual bool IsUseless() const override;

  // [R]
  public: virtual void Realize() override;

  // [U]
  public: virtual void Unrealize() override;

  DISALLOW_COPY_AND_ASSIGN(OpenInstruction);
}; // OpenInstruction

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_OpenInstruction_h)
