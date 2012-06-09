// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_PhiI_h)
#define INCLUDE_Il_Ir_Instructions_PhiI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

class PhiOperandBox;

// PhiI ty %rd <= (BBk %sx)+
class PhiI : public Instruction_<PhiI, Op_Phi> {
  // ctor
  public: PhiI();
  public: PhiI(const Type&, const Output&);

  // [A]
  public: void AddOperand(const BBlock&, const Operand&);
  public: void AddOperand(const BBlock*, const Operand*);

  // [F]
  public: PhiOperandBox* FindOperandBox(const BBlock*) const;

  // [G]
  public: PhiOperandBox* GetOperandBox(const BBlock*) const;
  public: Operand* GetOperand(const BBlock*) const;
  public: Register* GetRx(const BBlock*) const;

  DISALLOW_COPY_AND_ASSIGN(PhiI);
}; // PhiI

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_PhiI_h)
